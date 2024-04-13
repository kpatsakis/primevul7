    bool Converter::prepareExifTarget(const char* to, bool force)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(to));
        if (pos == exifData_->end()) return true;
        if (!overwrite_ && !force) return false;
        exifData_->erase(pos);
        return true;
    }