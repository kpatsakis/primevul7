    int XmpParser::decode(      XmpData&     xmpData,
                          const std::string& xmpPacket)
    {
        xmpData.clear();
        if (!xmpPacket.empty()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "XMP toolkit support not compiled in.\n";
#endif
        }
        return 1;
    } // XmpParser::decode