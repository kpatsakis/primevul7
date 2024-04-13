    int TiffImage::pixelHeight() const
    {
        if (pixelHeight_ != 0) return pixelHeight_;

        ExifKey key(std::string("Exif.") + primaryGroup() + std::string(".ImageLength"));
        ExifData::const_iterator imageHeight = exifData_.findKey(key);
        if (imageHeight != exifData_.end() && imageHeight->count() > 0) {
            pixelHeight_ = imageHeight->toLong();
        }
        return pixelHeight_;
    }