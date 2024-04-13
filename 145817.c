    void moveIptcToXmp(IptcData& iptcData, XmpData& xmpData, const char *iptcCharset)
    {
        if (!iptcCharset) iptcCharset = iptcData.detectCharset();
        if (!iptcCharset) iptcCharset = "ISO-8859-1";
        Converter converter(iptcData, xmpData, iptcCharset);
        converter.setErase();
        converter.cnvToXmp();
    }