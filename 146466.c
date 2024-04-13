    int TiffImage::pixelWidth() const
    {
        if (pixelWidth_ != 0) return pixelWidth_;

        ExifKey key(std::string("Exif.") + primaryGroup() + std::string(".ImageWidth"));
        ExifData::const_iterator imageWidth = exifData_.findKey(key);
        if (imageWidth != exifData_.end() && imageWidth->count() > 0) {
            pixelWidth_ = static_cast<int>(imageWidth->toLong());
        }
        return pixelWidth_;
    }