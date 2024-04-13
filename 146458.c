    bool Image::is8ByteType(uint16_t type)
    {
        return isRationalType(type)
             || isLongLongType(type)
             || type == Exiv2::tiffIfd8
             || type == Exiv2::tiffDouble
            ;
    }