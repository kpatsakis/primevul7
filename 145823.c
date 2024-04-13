    void ExifThumb::setJpegThumbnail(const byte* buf, long size)
    {
        exifData_["Exif.Thumbnail.Compression"] = uint16_t(6);
        Exifdatum& format = exifData_["Exif.Thumbnail.JPEGInterchangeFormat"];
        format = uint32_t(0);
        format.setDataArea(buf, size);
        exifData_["Exif.Thumbnail.JPEGInterchangeFormatLength"] = uint32_t(size);
    }