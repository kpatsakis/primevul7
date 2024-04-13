    bool Photoshop::isIrb(const byte* pPsData,
                          long        sizePsData)
    {
        if (sizePsData < 4) return false;
        for (size_t i = 0; i < (sizeof irbId_) / (sizeof *irbId_); i++) {
            assert(strlen(irbId_[i]) == 4);
            if (memcmp(pPsData, irbId_[i], 4) == 0) return true;
        }
        return false;
    }