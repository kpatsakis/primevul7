    TiffComponent* TiffMnCreator::create(uint16_t           tag,
                                         IfdId              group,
                                         const std::string& make,
                                         const byte*        pData,
                                         uint32_t           size,
                                         ByteOrder          byteOrder)
    {
        TiffComponent* tc = 0;
        const TiffMnRegistry* tmr = find(registry_, make);
        if (tmr) {
            assert(tmr->newMnFct_);
            tc = tmr->newMnFct_(tag,
                                group,
                                tmr->mnGroup_,
                                pData,
                                size,
                                byteOrder);
        }
        return tc;
    } // TiffMnCreator::create