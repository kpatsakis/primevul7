    uint32_t TiffComponent::write(IoWrapper& ioWrapper,
                                  ByteOrder byteOrder,
                                  int32_t   offset,
                                  uint32_t  valueIdx,
                                  uint32_t  dataIdx,
                                  uint32_t& imageIdx)
    {
        return doWrite(ioWrapper, byteOrder, offset, valueIdx, dataIdx, imageIdx);
    } // TiffComponent::write