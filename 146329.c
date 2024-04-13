    LangAltValue::LangAltValue(const std::string& buf)
        : XmpValue(langAlt)
    {
        read(buf);
    }