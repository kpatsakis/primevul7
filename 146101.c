    LoaderExifJpeg::LoaderExifJpeg(PreviewId id, const Image &image, int parIdx)
        : Loader(id, image)
    {
        offset_ = 0;
        const ExifData &exifData = image_.exifData();
        ExifData::const_iterator pos = exifData.findKey(ExifKey(param_[parIdx].offsetKey_));
        if (pos != exifData.end() && pos->count() > 0) {
            offset_ = pos->toLong();
        }

        size_ = 0;
        pos = exifData.findKey(ExifKey(param_[parIdx].sizeKey_));
        if (pos != exifData.end() && pos->count() > 0) {
            size_ = pos->toLong();
        }

        if (offset_ == 0 || size_ == 0) return;

        if (param_[parIdx].baseOffsetKey_) {
            pos = exifData.findKey(ExifKey(param_[parIdx].baseOffsetKey_));
            if (pos != exifData.end() && pos->count() > 0) {
                offset_ += pos->toLong();
            }
        }

        if (Safe::add(offset_, size_) > static_cast<uint32_t>(image_.io().size()))
            return;

        valid_ = true;
    }