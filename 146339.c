    CommentValue::CommentValue(const std::string& comment)
        : StringValueBase(Exiv2::undefined), byteOrder_(littleEndian)
    {
        read(comment);
    }