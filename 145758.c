    bool Image::isRationalType(uint16_t type) {
         return type == Exiv2::unsignedRational
             || type == Exiv2::signedRational
             ;
    }