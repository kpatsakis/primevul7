    void copyIptcToXmp(const IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
    {
        if (!iptcCharset) iptcCharset = iptcData.detectCharset();
        if (!iptcCharset) iptcCharset = "ISO-8859-1";

        Converter converter(const_cast<IptcData&>(iptcData), xmpData, iptcCharset);
        converter.cnvToXmp();
    }