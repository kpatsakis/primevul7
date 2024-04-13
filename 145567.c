    void ExifThumb::setJpegThumbnail(
        const std::wstring& wpath,
              URational     xres,
              URational     yres,
              uint16_t      unit
    )
    {
        DataBuf thumb = readFile(wpath); // may throw
        setJpegThumbnail(thumb.pData_, thumb.size_, xres, yres, unit);
    }