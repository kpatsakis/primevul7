    Exiv2::DataBuf JpegThumbnail::copy(const Exiv2::ExifData& exifData) const
    {
        Exiv2::ExifKey key("Exif.Thumbnail.JPEGInterchangeFormat");
        Exiv2::ExifData::const_iterator format = exifData.findKey(key);
        if (format == exifData.end()) return Exiv2::DataBuf();
        return format->dataArea();
    }