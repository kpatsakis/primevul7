    void Converter::cnvXmpComment(const char* from, const char* to)
    {
        if (!prepareExifTarget(to)) return;
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        std::string value;
        if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Assumes the XMP value is encoded in UTF-8, as it should be
        (*exifData_)[to] = "charset=Unicode " + value;
        if (erase_) xmpData_->erase(pos);
    }