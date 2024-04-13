    bool TiffMnRegistry::operator==(const std::string& key) const
    {
        std::string make(make_);
        if (key.size() > 0 && key[0] == '-') return false;
        return make == key.substr(0, make.length());
    }