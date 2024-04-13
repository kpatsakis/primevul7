    uint32_t TiffDirectory::doSize() const
    {
        uint32_t compCount = count();
        // Size of the directory, without values and additional data
        uint32_t len = 2 + 12 * compCount + (hasNext_ ? 4 : 0);
        // Size of IFD values and data
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            uint32_t sv = (*i)->size();
            if (sv > 4) {
                sv += sv & 1;               // Align value to word boundary
                len += sv;
            }
            uint32_t sd = (*i)->sizeData();
            sd += sd & 1;                   // Align data to word boundary
            len += sd;
        }
        // Size of next-IFD, if any
        uint32_t sizeNext = 0;
        if (pNext_) {
            sizeNext = pNext_->size();
            len += sizeNext;
        }
        // Reset size of IFD if it has no entries and no or empty next IFD.
        if (compCount == 0 && sizeNext == 0) len = 0;
        return len;
    } // TiffDirectory::doSize