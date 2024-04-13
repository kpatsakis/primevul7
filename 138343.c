void LibarchivePlugin::copyData(const QString& filename, struct archive *source, struct archive *dest, bool partialprogress)
{
    char buff[10240];

    auto readBytes = archive_read_data(source, buff, sizeof(buff));
    while (readBytes > 0 && !QThread::currentThread()->isInterruptionRequested()) {
        archive_write_data(dest, buff, static_cast<size_t>(readBytes));
        if (archive_errno(dest) != ARCHIVE_OK) {
            qCCritical(ARK) << "Error while extracting" << filename << ":" << archive_error_string(dest)
                            << "(error no =" << archive_errno(dest) << ')';
            return;
        }

        if (partialprogress) {
            m_currentExtractedFilesSize += readBytes;
            emit progress(float(m_currentExtractedFilesSize) / m_extractedFilesSize);
        }

        readBytes = archive_read_data(source, buff, sizeof(buff));
    }
}