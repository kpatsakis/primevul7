    LoaderExifDataJpeg::LoaderExifDataJpeg(PreviewId id, const Image &image, int parIdx)
        : Loader(id, image),
          dataKey_(param_[parIdx].dataKey_)
    {
        ExifData::const_iterator pos = image_.exifData().findKey(dataKey_);
        if (pos != image_.exifData().end()) {
            size_ = pos->sizeDataArea(); // indirect data
            if (size_ == 0 && pos->typeId() == undefined)
                size_ = pos->size(); // direct data
        }

        if (size_ == 0) return;

        valid_ = true;
    }