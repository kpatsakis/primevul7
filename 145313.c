    int Photoshop::locatePreviewIrb(const byte*     pPsData,
                                    size_t sizePsData,
                                    const byte**    record,
                                    uint32_t *const sizeHdr,
                                    uint32_t *const sizeData)
    {
        return locateIrb(pPsData, sizePsData, preview_, record, sizeHdr, sizeData);
    }