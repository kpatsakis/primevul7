    void Converter::cnvXmpValue(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value;
        if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Todo: Escape non-ASCII characters in XMP text values
        ExifKey key(to);
        Exifdatum ed(key);
        if (0 == ed.setValue(value)) {
            exifData_->add(ed);
        }
        if (erase_) xmpData_->erase(pos);
    }