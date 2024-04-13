    uint32_t TiffIfdMakernote::doSize() const
    {
        return sizeHeader() + ifd_.size();
    } // TiffIfdMakernote::doSize