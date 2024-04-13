    int nikonSelector(uint16_t tag, const byte* pData, uint32_t size, TiffComponent* const /*pRoot*/)
    {
        if (size < 4) return -1;
        const NikonArrayIdx* aix = find(nikonArrayIdx, NikonArrayIdx::Key(tag, reinterpret_cast<const char*>(pData), size));
        return aix == 0 ? -1 : aix->idx_;
    }