    std::string TiffImage::mimeType() const
    {
        if (!mimeType_.empty()) return mimeType_;

        mimeType_ = std::string("image/tiff");
        std::string key = "Exif." + primaryGroup() + ".Compression";
        ExifData::const_iterator md = exifData_.findKey(ExifKey(key));
        if (md != exifData_.end() && md->count() > 0) {
            const MimeTypeList* i = find(mimeTypeList, static_cast<int>(md->toLong()));
            if (i) mimeType_ = std::string(i->mimeType_);
        }
        return mimeType_;
    }