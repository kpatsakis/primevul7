    const char* Image::typeName(uint16_t tag) const
    {
        //! List of TIFF image tags
        const char* result = nullptr;
        switch (tag ) {
            case Exiv2::unsignedByte     : result = "BYTE"      ; break;
            case Exiv2::asciiString      : result = "ASCII"     ; break;
            case Exiv2::unsignedShort    : result = "SHORT"     ; break;
            case Exiv2::unsignedLong     : result = "LONG"      ; break;
            case Exiv2::unsignedRational : result = "RATIONAL"  ; break;
            case Exiv2::signedByte       : result = "SBYTE"     ; break;
            case Exiv2::undefined        : result = "UNDEFINED" ; break;
            case Exiv2::signedShort      : result = "SSHORT"    ; break;
            case Exiv2::signedLong       : result = "SLONG"     ; break;
            case Exiv2::signedRational   : result = "SRATIONAL" ; break;
            case Exiv2::tiffFloat        : result = "FLOAT"     ; break;
            case Exiv2::tiffDouble       : result = "DOUBLE"    ; break;
            case Exiv2::tiffIfd          : result = "IFD"       ; break;
            default                      : result = "unknown"   ; break;
        }
        return result;
    }