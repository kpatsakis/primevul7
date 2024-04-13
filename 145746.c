    void ExifThumb::setJpegThumbnail(const std::string& path)
    {
        DataBuf thumb = readFile(path); // may throw
        setJpegThumbnail(thumb.pData_, (long)thumb.size_);
    }