    uint32_t TiffIfdMakernote::baseOffset() const
    {
        if (!pHeader_) return 0;
        return pHeader_->baseOffset(mnOffset_);
    }