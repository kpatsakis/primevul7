    bool Converter::prepareXmpTarget(const char* to, bool force)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(to));
        if (pos == xmpData_->end()) return true;
        if (!overwrite_ && !force) return false;
        xmpData_->erase(pos);
        return true;
    }