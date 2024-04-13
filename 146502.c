    Converter::Converter(IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
        : erase_(false), overwrite_(true), exifData_(0), iptcData_(&iptcData), xmpData_(&xmpData), iptcCharset_(iptcCharset)
    {
    }