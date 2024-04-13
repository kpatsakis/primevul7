    uint32_t TiffMnEntry::doSize() const
    {
        if (!mn_) {
            return TiffEntryBase::doSize();
        }
        return mn_->size();
    } // TiffMnEntry::doSize