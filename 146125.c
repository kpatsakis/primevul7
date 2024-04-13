    bool getTextValue(std::string& value, const XmpData::iterator& pos)
    {
        if (pos->typeId() == langAlt) {
            // get the default language entry without x-default qualifier
            value = pos->toString(0);
            if (!pos->value().ok() && pos->count() == 1) {
                // If there is no default but exactly one entry, take that
                // without the qualifier
                value = pos->toString();
                if (   pos->value().ok()
                    && value.length() > 5 && value.substr(0, 5) == "lang=") {
                    const std::string::size_type first_space_pos = value.find_first_of(' ');
                    if (first_space_pos != std::string::npos) {
                        value = value.substr(first_space_pos + 1);
                    }
                    else {
                        value.clear();
                    }
                }
            }
        }
        else {
            value = pos->toString();
        }
        return pos->value().ok();
    }