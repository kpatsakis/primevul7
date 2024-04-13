bool LibarchivePlugin::initializeReader()
{
    m_archiveReader.reset(archive_read_new());

    if (!(m_archiveReader.data())) {
        emit error(i18n("The archive reader could not be initialized."));
        return false;
    }

    if (archive_read_support_filter_all(m_archiveReader.data()) != ARCHIVE_OK) {
        return false;
    }

    if (archive_read_support_format_all(m_archiveReader.data()) != ARCHIVE_OK) {
        return false;
    }

    if (archive_read_open_filename(m_archiveReader.data(), QFile::encodeName(filename()).constData(), 10240) != ARCHIVE_OK) {
        qCWarning(ARK) << "Could not open the archive:" << archive_error_string(m_archiveReader.data());
        emit error(i18nc("@info", "Archive corrupted or insufficient permissions."));
        return false;
    }

    return true;
}