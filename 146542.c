    Iptcdatum& IptcData::operator[](const std::string& key)
    {
        IptcKey iptcKey(key);
        iterator pos = findKey(iptcKey);
        if (pos == end()) {
            add(Iptcdatum(iptcKey));
            pos = findKey(iptcKey);
        }
        return *pos;
    }