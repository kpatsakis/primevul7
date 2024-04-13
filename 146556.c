    void moveXmpToIptc(XmpData& xmpData, IptcData& iptcData)
    {
        Converter converter(iptcData, xmpData);
        converter.setErase();
        converter.cnvFromXmp();
    }