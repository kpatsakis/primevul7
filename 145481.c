    void Converter::cnvExifGPSVersion(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        std::ostringstream value;
        for (int i = 0; i < pos->count(); ++i) {
            if (i > 0) value << '.';
            value << pos->toLong(i);
        }
        (*xmpData_)[to] = value.str();
        if (erase_) exifData_->erase(pos);
    }