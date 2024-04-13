    ByteOrder ExifParser::decode(
              ExifData& exifData,
        const byte*     pData,
              uint32_t  size
    )
    {
        IptcData iptcData;
        XmpData  xmpData;
        ByteOrder bo = TiffParser::decode(exifData,
                                          iptcData,
                                          xmpData,
                                          pData,
                                          size);
#ifndef SUPPRESS_WARNINGS
        if (!iptcData.empty()) {
            EXV_WARNING << "Ignoring IPTC information encoded in the Exif data.\n";
        }
        if (!xmpData.empty()) {
            EXV_WARNING << "Ignoring XMP information encoded in the Exif data.\n";
        }
#endif
        return bo;
    } // ExifParser::decode