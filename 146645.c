    int LangAltValue::read(const std::string& buf)
    {
        std::string b = buf;
        std::string lang = "x-default";
        if (buf.length() > 5 && buf.substr(0, 5) == "lang=") {
            std::string::size_type pos = buf.find_first_of(' ');
            lang = buf.substr(5, pos-5);
            // Strip quotes (so you can also specify the language without quotes)
            if (lang[0] == '"') lang = lang.substr(1);
            if (lang[lang.length()-1] == '"') lang = lang.substr(0, lang.length()-1);
            b.clear();
            if (pos != std::string::npos) b = buf.substr(pos+1);
        }
        value_[lang] = b;
        return 0;
    }