    uint32_t TiffIfdMakernote::ifdOffset() const
    {
        if (!pHeader_) return 0;
        return pHeader_->ifdOffset();
    }