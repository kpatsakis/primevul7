    void copyXmpToIptc(const XmpData& xmpData, IptcData& iptcData)
    {
        Converter converter(iptcData, const_cast<XmpData&>(xmpData));
        converter.cnvFromXmp();
    }