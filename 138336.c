bool LibarchivePlugin::extractFiles(const QVector<Archive::Entry*> &files, const QString &destinationDirectory, const ExtractionOptions &options)
{
    if (!initializeReader()) {
        return false;
    }

    ArchiveWrite writer(archive_write_disk_new());
    if (!writer.data()) {
        return false;
    }

    archive_write_disk_set_options(writer.data(), extractionFlags());

    int totalEntriesCount = 0;
    const bool extractAll = files.isEmpty();
    if (extractAll) {
        if (!m_cachedArchiveEntryCount) {
            emit progress(0);
            //TODO: once information progress has been implemented, send
            //feedback here that the archive is being read
            qCDebug(ARK) << "For getting progress information, the archive will be listed once";
            m_emitNoEntries = true;
            list();
            m_emitNoEntries = false;
        }
        totalEntriesCount = m_cachedArchiveEntryCount;
    } else {
        totalEntriesCount = files.size();
    }

    qCDebug(ARK) << "Going to extract" << totalEntriesCount << "entries";

    qCDebug(ARK) << "Changing current directory to " << destinationDirectory;
    m_oldWorkingDir = QDir::currentPath();
    QDir::setCurrent(destinationDirectory);

    // Initialize variables.
    const bool preservePaths = options.preservePaths();
    const bool removeRootNode = options.isDragAndDropEnabled();
    bool overwriteAll = false; // Whether to overwrite all files
    bool skipAll = false; // Whether to skip all files
    bool dontPromptErrors = false; // Whether to prompt for errors
    m_currentExtractedFilesSize = 0;
    int extractedEntriesCount = 0;
    int progressEntryCount = 0;
    struct archive_entry *entry;
    QString fileBeingRenamed;
    // To avoid traversing the entire archive when extracting a limited set of
    // entries, we maintain a list of remaining entries and stop when it's empty.
    const QStringList fullPaths = entryFullPaths(files);
    QStringList remainingFiles = entryFullPaths(files);

    // Iterate through all entries in archive.
    while (!QThread::currentThread()->isInterruptionRequested() && (archive_read_next_header(m_archiveReader.data(), &entry) == ARCHIVE_OK)) {

        if (!extractAll && remainingFiles.isEmpty()) {
            break;
        }

        fileBeingRenamed.clear();
        int index = -1;

        // Retry with renamed entry, fire an overwrite query again
        // if the new entry also exists.
    retry:
        const bool entryIsDir = S_ISDIR(archive_entry_mode(entry));
        // Skip directories if not preserving paths.
        if (!preservePaths && entryIsDir) {
            archive_read_data_skip(m_archiveReader.data());
            continue;
        }

        // entryName is the name inside the archive, full path
        QString entryName = QDir::fromNativeSeparators(QFile::decodeName(archive_entry_pathname(entry)));

        // Some archive types e.g. AppImage prepend all entries with "./" so remove this part.
        if (entryName.startsWith(QLatin1String("./"))) {
            entryName.remove(0, 2);
        }

        // Static libraries (*.a) contain the two entries "/" and "//".
        // We just skip these to allow extracting this archive type.
        if (entryName == QLatin1String("/") || entryName == QLatin1String("//")) {
            archive_read_data_skip(m_archiveReader.data());
            continue;
        }

        // For now we just can't handle absolute filenames in a tar archive.
        // TODO: find out what to do here!!
        if (entryName.startsWith(QLatin1Char( '/' ))) {
            emit error(i18n("This archive contains archive entries with absolute paths, "
                            "which are not supported by Ark."));
            return false;
        }

        // Should the entry be extracted?
        if (extractAll ||
            remainingFiles.contains(entryName) ||
            entryName == fileBeingRenamed) {

            // Find the index of entry.
            if (entryName != fileBeingRenamed) {
                index = fullPaths.indexOf(entryName);
            }
            if (!extractAll && index == -1) {
                // If entry is not found in files, skip entry.
                continue;
            }

            // entryFI is the fileinfo pointing to where the file will be
            // written from the archive.
            QFileInfo entryFI(entryName);
            //qCDebug(ARK) << "setting path to " << archive_entry_pathname( entry );

            const QString fileWithoutPath(entryFI.fileName());
            // If we DON'T preserve paths, we cut the path and set the entryFI
            // fileinfo to the one without the path.
            if (!preservePaths) {
                // Empty filenames (ie dirs) should have been skipped already,
                // so asserting.
                Q_ASSERT(!fileWithoutPath.isEmpty());
                archive_entry_copy_pathname(entry, QFile::encodeName(fileWithoutPath).constData());
                entryFI = QFileInfo(fileWithoutPath);

            // OR, if the file has a rootNode attached, remove it from file path.
            } else if (!extractAll && removeRootNode && entryName != fileBeingRenamed) {
                const QString &rootNode = files.at(index)->rootNode;
                if (!rootNode.isEmpty()) {
                    const QString truncatedFilename(entryName.remove(entryName.indexOf(rootNode), rootNode.size()));
                    archive_entry_copy_pathname(entry, QFile::encodeName(truncatedFilename).constData());
                    entryFI = QFileInfo(truncatedFilename);
                }
            }

            // Check if the file about to be written already exists.
            if (!entryIsDir && entryFI.exists()) {
                if (skipAll) {
                    archive_read_data_skip(m_archiveReader.data());
                    archive_entry_clear(entry);
                    continue;
                } else if (!overwriteAll && !skipAll) {
                    Kerfuffle::OverwriteQuery query(entryName);
                    emit userQuery(&query);
                    query.waitForResponse();

                    if (query.responseCancelled()) {
                        emit cancelled();
                        archive_read_data_skip(m_archiveReader.data());
                        archive_entry_clear(entry);
                        break;
                    } else if (query.responseSkip()) {
                        archive_read_data_skip(m_archiveReader.data());
                        archive_entry_clear(entry);
                        continue;
                    } else if (query.responseAutoSkip()) {
                        archive_read_data_skip(m_archiveReader.data());
                        archive_entry_clear(entry);
                        skipAll = true;
                        continue;
                    } else if (query.responseRename()) {
                        const QString newName(query.newFilename());
                        fileBeingRenamed = newName;
                        archive_entry_copy_pathname(entry, QFile::encodeName(newName).constData());
                        goto retry;
                    } else if (query.responseOverwriteAll()) {
                        overwriteAll = true;
                    }
                }
            }

            // If there is an already existing directory.
            if (entryIsDir && entryFI.exists()) {
                if (entryFI.isWritable()) {
                    qCWarning(ARK) << "Warning, existing, but writable dir";
                } else {
                    qCWarning(ARK) << "Warning, existing, but non-writable dir. skipping";
                    archive_entry_clear(entry);
                    archive_read_data_skip(m_archiveReader.data());
                    continue;
                }
            }

            // Write the entry header and check return value.
            const int returnCode = archive_write_header(writer.data(), entry);
            switch (returnCode) {
            case ARCHIVE_OK:
                // If the whole archive is extracted and the total filesize is
                // available, we use partial progress.
                copyData(entryName, m_archiveReader.data(), writer.data(), (extractAll && m_extractedFilesSize));
                break;

            case ARCHIVE_FAILED:
                qCCritical(ARK) << "archive_write_header() has returned" << returnCode
                                << "with errno" << archive_errno(writer.data());

                // If they user previously decided to ignore future errors,
                // don't bother prompting again.
                if (!dontPromptErrors) {
                    // Ask the user if he wants to continue extraction despite an error for this entry.
                    Kerfuffle::ContinueExtractionQuery query(QLatin1String(archive_error_string(writer.data())),
                                                             entryName);
                    emit userQuery(&query);
                    query.waitForResponse();

                    if (query.responseCancelled()) {
                        emit cancelled();
                        return false;
                    }
                    dontPromptErrors = query.dontAskAgain();
                }
                break;

            case ARCHIVE_FATAL:
                qCCritical(ARK) << "archive_write_header() has returned" << returnCode
                                << "with errno" << archive_errno(writer.data());
                emit error(i18nc("@info", "Fatal error, extraction aborted."));
                return false;
            default:
                qCDebug(ARK) << "archive_write_header() returned" << returnCode
                             << "which will be ignored.";
                break;
            }

            // If we only partially extract the archive and the number of
            // archive entries is available we use a simple progress based on
            // number of items extracted.
            if (!extractAll && m_cachedArchiveEntryCount) {
                ++progressEntryCount;
                emit progress(float(progressEntryCount) / totalEntriesCount);
            }

            extractedEntriesCount++;
            remainingFiles.removeOne(entryName);
        } else {
            // Archive entry not among selected files, skip it.
            archive_read_data_skip(m_archiveReader.data());
        }
    }

    qCDebug(ARK) << "Extracted" << extractedEntriesCount << "entries";
    slotRestoreWorkingDir();
    return archive_read_close(m_archiveReader.data()) == ARCHIVE_OK;
}