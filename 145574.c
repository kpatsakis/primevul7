    std::string& Image::xmpPacket()
    {
        // Serialize the current XMP
        if (xmpData_.count() > 0 && !writeXmpFromPacket()) {
            XmpParser::encode(xmpPacket_, xmpData_, XmpParser::useCompactFormat | XmpParser::omitAllFormatting);
        }
        return xmpPacket_;
    }