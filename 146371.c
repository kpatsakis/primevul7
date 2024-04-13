    int Photoshop::locateIptcIrb(const byte*     pPsData,
                                 size_t sizePsData,
                                 const byte**    record,
                                 uint32_t *const sizeHdr,
                                 uint32_t *const sizeData)
    {
        return locateIrb(pPsData, sizePsData, iptc_, record, sizeHdr, sizeData);
    }