    bool TiffEncoder::isImageTag(uint16_t tag, IfdId group) const
    {
        if (!isNewImage_ && pHeader_->isImageTag(tag, group, pPrimaryGroups_)) {
            return true;
        }
        return false;
    }