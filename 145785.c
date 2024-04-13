    int CommentValue::read(const std::string& comment)
    {
        std::string c = comment;
        CharsetId charsetId = undefined;
        if (comment.length() > 8 && comment.substr(0, 8) == "charset=") {
            std::string::size_type pos = comment.find_first_of(' ');
            std::string name = comment.substr(8, pos-8);
            // Strip quotes (so you can also specify the charset without quotes)
            if (name[0] == '"') name = name.substr(1);
            if (name[name.length()-1] == '"') name = name.substr(0, name.length()-1);
            charsetId = CharsetInfo::charsetIdByName(name);
            if (charsetId == invalidCharsetId) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << Error(kerInvalidCharset, name) << "\n";
#endif
                return 1;
            }
            c.clear();
            if (pos != std::string::npos) c = comment.substr(pos+1);
        }
        if (charsetId == unicode) {
            const char* to = byteOrder_ == littleEndian ? "UCS-2LE" : "UCS-2BE";
            convertStringCharset(c, "UTF-8", to);
        }
        const std::string code(CharsetInfo::code(charsetId), 8);
        return StringValueBase::read(code + c);
    }