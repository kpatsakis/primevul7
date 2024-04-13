    uint32_t TiffImageEntry::doSizeImage() const
    {
        if (!pValue()) return 0;
        uint32_t len = pValue()->sizeDataArea();
        if (len == 0) {
            for (Strips::const_iterator i = strips_.begin(); i != strips_.end(); ++i) {
                len += i->second;
            }
        }
        return len;
    } // TiffImageEntry::doSizeImage