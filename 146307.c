    void ExifThumb::setJpegThumbnail(const byte* buf, size_t size, URational xres, URational yres, uint16_t unit)
    {
        setJpegThumbnail(buf, (long)size);
        exifData_["Exif.Thumbnail.XResolution"] = xres;
        exifData_["Exif.Thumbnail.YResolution"] = yres;
        exifData_["Exif.Thumbnail.ResolutionUnit"] = unit;
    }