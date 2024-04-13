    void Converter::cnvExifValue(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        if (!prepareXmpTarget(to)) return;
        (*xmpData_)[to] = value;
        if (erase_) exifData_->erase(pos);
    }