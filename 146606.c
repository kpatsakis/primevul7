    bool Image::isLongLongType(uint16_t type) {
        return type == Exiv2::unsignedLongLong
            || type == Exiv2::signedLongLong
            ;
    }