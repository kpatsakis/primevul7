    std::string CommentValue::comment(const char* encoding) const
    {
        std::string c;
        if (value_.length() < 8) {
            return c;
        }
        c = value_.substr(8);
        if (charsetId() == unicode) {
            const char* from = encoding == 0 || *encoding == '\0' ? detectCharset(c) : encoding;
            convertStringCharset(c, from, "UTF-8");
        }
        return c;
    }