    uint32_t TiffIfdMakernote::sizeHeader() const
    {
        if (!pHeader_) return 0;
        return (uint32_t)pHeader_->size();
    }