    void Converter::cnvXmpGPSVersion(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        for (unsigned i = 0; i < value.length(); ++i) {
            if (value[i] == '.') value[i] = ' ';
        }
        (*exifData_)[to] = value;
        if (erase_) xmpData_->erase(pos);

    }