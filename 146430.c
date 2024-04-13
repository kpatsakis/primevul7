    bool Converter::prepareIptcTarget(const char* to, bool force)
    {
        Exiv2::IptcData::iterator pos = iptcData_->findKey(IptcKey(to));
        if (pos == iptcData_->end()) return true;
        if (!overwrite_ && !force) return false;
        while ((pos = iptcData_->findKey(IptcKey(to))) != iptcData_->end()) {
            iptcData_->erase(pos);
        }
        return true;
    }