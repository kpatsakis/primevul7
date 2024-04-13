    XmpTextValue::XmpTextValue(const std::string& buf)
        : XmpValue(xmpText)
    {
        read(buf);
    }