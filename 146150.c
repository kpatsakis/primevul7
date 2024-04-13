    LoaderTiff::LoaderTiff(PreviewId id, const Image &image, int parIdx)
        : Loader(id, image),
          group_(param_[parIdx].group_)
    {
        const ExifData &exifData = image_.exifData();

        int offsetCount = 0;
        ExifData::const_iterator pos;

        // check if the group_ contains a preview image
        if (param_[parIdx].checkTag_) {
            pos = exifData.findKey(ExifKey(param_[parIdx].checkTag_));
            if (pos == exifData.end()) return;
            if (param_[parIdx].checkValue_ && pos->toString() != param_[parIdx].checkValue_) return;
        }

        pos = exifData.findKey(ExifKey(std::string("Exif.") + group_ + ".StripOffsets"));
        if (pos != exifData.end()) {
            offsetTag_ = "StripOffsets";
            sizeTag_ = "StripByteCounts";
            offsetCount = pos->value().count();
        }
        else {
            pos = exifData.findKey(ExifKey(std::string("Exif.") + group_ + ".TileOffsets"));
            if (pos == exifData.end()) return;
            offsetTag_ = "TileOffsets";
            sizeTag_ = "TileByteCounts";
            offsetCount = pos->value().count();
        }

        pos = exifData.findKey(ExifKey(std::string("Exif.") + group_ + '.' + sizeTag_));
        if (pos == exifData.end()) return;
        if (offsetCount != pos->value().count()) return;
        for (int i = 0; i < offsetCount; i++) {
            size_ += pos->toLong(i);
        }

        if (size_ == 0) return;

        pos = exifData.findKey(ExifKey(std::string("Exif.") + group_ + ".ImageWidth"));
        if (pos != exifData.end() && pos->count() > 0) {
            width_ = pos->toLong();
        }

        pos = exifData.findKey(ExifKey(std::string("Exif.") + group_ + ".ImageLength"));
        if (pos != exifData.end() && pos->count() > 0) {
            height_ = pos->toLong();
        }

        if (width_ == 0 || height_ == 0) return;

        valid_ = true;
    }