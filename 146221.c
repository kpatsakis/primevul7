    void Converter::cnvXmpValueToIptc(const char* from, const char* to)
    {
        XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareIptcTarget(to)) return;

        if (pos->typeId() == langAlt || pos->typeId() == xmpText) {
            std::string value;
            if (!getTextValue(value, pos)) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            (*iptcData_)[to] = value;
            (*iptcData_)["Iptc.Envelope.CharacterSet"] = "\033%G"; // indicate UTF-8 encoding
            if (erase_) xmpData_->erase(pos);
            return;
        }

        const size_t count = pos->count();
        bool added = false;
        for (int i = 0; i < count; ++i) {
            std::string value = pos->toString(i);
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                continue;
            }
            IptcKey key(to);
            Iptcdatum id(key);
            id.setValue(value);
            iptcData_->add(id);
            added = true;
        }
        if (added) (*iptcData_)["Iptc.Envelope.CharacterSet"] = "\033%G"; // indicate UTF-8 encoding
        if (erase_) xmpData_->erase(pos);
    }