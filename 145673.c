    Exiv2::ByteOrder stringToByteOrder(const std::string& val)
    {
        Exiv2::ByteOrder bo = Exiv2::invalidByteOrder;
        if (0 == strcmp("II", val.c_str())) bo = Exiv2::littleEndian;
        else if (0 == strcmp("MM", val.c_str())) bo = Exiv2::bigEndian;

        return bo;
    }