bool LibarchivePlugin::emitCorruptArchive()
{
    Kerfuffle::LoadCorruptQuery query(filename());
    emit userQuery(&query);
    query.waitForResponse();
    if (!query.responseYes()) {
        emit cancelled();
        archive_read_close(m_archiveReader.data());
        return false;
    } else {
        emit progress(1.0);
        return true;
    }
}