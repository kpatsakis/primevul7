    TiffDirectory::TiffDirectory(const TiffDirectory& rhs)
        : TiffComponent(rhs),
          hasNext_(rhs.hasNext_),
          pNext_(0)
    {
    }