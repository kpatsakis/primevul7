    void CrwImage::setIptcData(const IptcData& /*iptcData*/)
    {
        // not supported
        throw(Error(kerInvalidSettingForImage, "IPTC metadata", "CRW"));
    }