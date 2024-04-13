    uint32_t TiffDataEntry::doSizeData() const
    {
        if (!pValue()) return 0;
        return pValue()->sizeDataArea();
    } // TiffDataEntry::doSizeData