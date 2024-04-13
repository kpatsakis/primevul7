    bool Image::is4ByteType(uint16_t type)
    {
        return isLongType(type)
            || type == Exiv2::tiffFloat
            || type == Exiv2::tiffIfd
            ;
    }