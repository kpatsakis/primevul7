    TiffIfdMakernote::TiffIfdMakernote(uint16_t  tag,
                                       IfdId     group,
                                       IfdId     mnGroup,
                                       MnHeader* pHeader,
                                       bool      hasNext)
        : TiffComponent(tag, group),
          pHeader_(pHeader),
          ifd_(tag, mnGroup, hasNext),
          mnOffset_(0),
          imageByteOrder_(invalidByteOrder)
    {
    }