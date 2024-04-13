    void Converter::cnvXmpArray(const char* from, const char* to)
    {
        if (!prepareExifTarget(to)) return;
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        std::ostringstream array;
        for (int i = 0; i < pos->count(); ++i) {
            std::string value = pos->toString(i);
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            array << value;
            if (i != pos->count() - 1) array << " ";
        }
        (*exifData_)[to] = array.str();
        if (erase_) xmpData_->erase(pos);
    }