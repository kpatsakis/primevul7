    void ExifThumb::setJpegThumbnail(const std::wstring& wpath)
    {
        DataBuf thumb = readFile(wpath); // may throw
        setJpegThumbnail(thumb.pData_, thumb.size_);
    }