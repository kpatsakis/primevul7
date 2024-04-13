bool LibarchivePlugin::list()
{
    qCDebug(ARK) << "Listing archive contents";

    if (!initializeReader()) {
        return false;
    }

    qCDebug(ARK) << "Detected compression filter:" << archive_filter_name(m_archiveReader.data(), 0);
    QString compMethod = convertCompressionName(QString::fromUtf8(archive_filter_name(m_archiveReader.data(), 0)));
    if (!compMethod.isEmpty()) {
        emit compressionMethodFound(compMethod);
    }

    m_cachedArchiveEntryCount = 0;
    m_extractedFilesSize = 0;
    m_numberOfEntries = 0;
    auto compressedArchiveSize = QFileInfo(filename()).size();

    struct archive_entry *aentry;
    int result = ARCHIVE_RETRY;

    bool firstEntry = true;
    while (!QThread::currentThread()->isInterruptionRequested() && (result = archive_read_next_header(m_archiveReader.data(), &aentry)) == ARCHIVE_OK) {

        if (firstEntry) {
            qCDebug(ARK) << "Detected format for first entry:" << archive_format_name(m_archiveReader.data());
            firstEntry = false;
        }

        if (!m_emitNoEntries) {
            emitEntryFromArchiveEntry(aentry);
        }

        m_extractedFilesSize += (qlonglong)archive_entry_size(aentry);

        emit progress(float(archive_filter_bytes(m_archiveReader.data(), -1))/float(compressedArchiveSize));

        m_cachedArchiveEntryCount++;

        // Skip the entry data.
        int readSkipResult = archive_read_data_skip(m_archiveReader.data());
        if (readSkipResult != ARCHIVE_OK) {
            qCCritical(ARK) << "Error while skipping data for entry:"
                            << QString::fromWCharArray(archive_entry_pathname_w(aentry))
                            << readSkipResult
                            << QLatin1String(archive_error_string(m_archiveReader.data()));
            if (!emitCorruptArchive()) {
                return false;
            }
        }
    }

    if (result != ARCHIVE_EOF) {
        qCCritical(ARK) << "Error while reading archive:"
                        << result
                        << QLatin1String(archive_error_string(m_archiveReader.data()));
        if (!emitCorruptArchive()) {
            return false;
        }
    }

    return archive_read_close(m_archiveReader.data()) == ARCHIVE_OK;
}