    XMP_OptionBits xmpFormatOptionBits(Exiv2::XmpParser::XmpFormatFlags flags)
    {
        XMP_OptionBits var(0);
        if (flags & Exiv2::XmpParser::omitPacketWrapper)   var |= kXMP_OmitPacketWrapper;
        if (flags & Exiv2::XmpParser::readOnlyPacket)      var |= kXMP_ReadOnlyPacket;
        if (flags & Exiv2::XmpParser::useCompactFormat)    var |= kXMP_UseCompactFormat;
        if (flags & Exiv2::XmpParser::includeThumbnailPad) var |= kXMP_IncludeThumbnailPad;
        if (flags & Exiv2::XmpParser::exactPacketLength)   var |= kXMP_ExactPacketLength;
        if (flags & Exiv2::XmpParser::writeAliasComments)  var |= kXMP_WriteAliasComments;
        if (flags & Exiv2::XmpParser::omitAllFormatting)   var |= kXMP_OmitAllFormatting;
        return var;
    }