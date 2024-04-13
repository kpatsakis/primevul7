    bool Image::isStringType(uint16_t type)
    {
        return type == Exiv2::asciiString
            || type == Exiv2::unsignedByte
            || type == Exiv2::signedByte
            || type == Exiv2::undefined
            ;
    }