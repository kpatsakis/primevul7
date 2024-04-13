    TiffEntryBase::TiffEntryBase(uint16_t tag, IfdId group, TiffType tiffType)
        : TiffComponent(tag, group),
          tiffType_(tiffType), count_(0), offset_(0),
          size_(0), pData_(0), isMalloced_(false), idx_(0),
          pValue_(0)
    {
    }