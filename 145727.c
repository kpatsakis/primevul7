    bool Photoshop::valid(const byte* pPsData,
                          size_t sizePsData)
    {
        const byte *record = 0;
        uint32_t sizeIptc = 0;
        uint32_t sizeHdr = 0;
        const byte* pCur = pPsData;
        const byte* pEnd = pPsData + sizePsData;
        int ret = 0;
        while (pCur < pEnd
               && 0 == (ret = Photoshop::locateIptcIrb(pCur, static_cast<long>(pEnd - pCur),
                                                       &record, &sizeHdr, &sizeIptc))) {
            pCur = record + sizeHdr + sizeIptc + (sizeIptc & 1);
        }
        return ret >= 0;
    }