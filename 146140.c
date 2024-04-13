    const char* CommentValue::detectCharset(std::string& c) const
    {
        // Interpret a BOM if there is one
        if (0 == strncmp(c.data(), "\xef\xbb\xbf", 3)) {
            c = c.substr(3);
            return "UTF-8";
        }
        if (0 == strncmp(c.data(), "\xff\xfe", 2)) {
            c = c.substr(2);
            return "UCS-2LE";
        }
        if (0 == strncmp(c.data(), "\xfe\xff", 2)) {
            c = c.substr(2);
            return "UCS-2BE";
        }

        // Todo: Add logic to guess if the comment is encoded in UTF-8

        return byteOrder_ == littleEndian ? "UCS-2LE" : "UCS-2BE";
    }