    void Converter::cnvIptcValue(const char* from, const char* to)
    {
        Exiv2::IptcData::iterator pos = iptcData_->findKey(IptcKey(from));
        if (pos == iptcData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        while (pos != iptcData_->end()) {
            if (pos->key() == from) {
                std::string value = pos->toString();
                if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                    EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                    ++pos;
                    continue;
                }
                if (iptcCharset_) convertStringCharset(value, iptcCharset_, "UTF-8");
                (*xmpData_)[to] = value;
                if (erase_) {
                    pos = iptcData_->erase(pos);
                    continue;
                }
            }
            ++pos;
        }
    }