    int XmpTextValue::read(const std::string& buf)
    {
        // support a type=Alt,Bag,Seq,Struct indicator
        std::string b = buf;
        std::string type;
        if (buf.length() > 5 && buf.substr(0, 5) == "type=") {
            std::string::size_type pos = buf.find_first_of(' ');
            type = buf.substr(5, pos-5);
            // Strip quotes (so you can also specify the type without quotes)
            if (type[0] == '"') type = type.substr(1);
            if (type[type.length()-1] == '"') type = type.substr(0, type.length()-1);
            b.clear();
            if (pos != std::string::npos) b = buf.substr(pos+1);
        }
        if (!type.empty()) {
            if (type == "Alt") {
                setXmpArrayType(XmpValue::xaAlt);
            }
            else if (type == "Bag") {
                setXmpArrayType(XmpValue::xaBag);
            }
            else if (type == "Seq") {
                setXmpArrayType(XmpValue::xaSeq);
            }
            else if (type == "Struct") {
                setXmpStruct();
            }
            else {
                throw Error(kerInvalidXmpText, type);
            }
        }
        value_ = b;
        return 0;
    }