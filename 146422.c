    TiffEntryBase::TiffEntryBase(const TiffEntryBase& rhs)
        : TiffComponent(rhs),
          tiffType_(rhs.tiffType_),
          count_(rhs.count_),
          offset_(rhs.offset_),
          size_(rhs.size_),
          pData_(rhs.pData_),
          isMalloced_(rhs.isMalloced_),
          idx_(rhs.idx_),
          pValue_(rhs.pValue_ ? rhs.pValue_->clone().release() : 0)
    {
        if (rhs.isMalloced_) {
            pData_ = new byte[rhs.size_];
            memcpy(pData_, rhs.pData_, rhs.size_);
        }
    }