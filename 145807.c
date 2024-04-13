    void Converter::cnvXmpVersion(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok() || value.length() < 4) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        std::ostringstream array;

        array << static_cast<int>(value[0]) << " "
              << static_cast<int>(value[1]) << " "
              << static_cast<int>(value[2]) << " "
              << static_cast<int>(value[3]);

        (*exifData_)[to] = array.str();
        if (erase_) xmpData_->erase(pos);
    }