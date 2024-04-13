    int XmpParser::encode(      std::string& /*xmpPacket*/,
                          const XmpData&     xmpData,
                                uint16_t     /*formatFlags*/,
                                uint32_t     /*padding*/)
    {
        if (!xmpData.empty()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "XMP toolkit support not compiled in.\n";
#endif
        }
        return 1;
    } // XmpParser::encode