    uint32_t TiffComponent::writeData(IoWrapper& ioWrapper,
                                      ByteOrder byteOrder,
                                      int32_t   offset,
                                      uint32_t  dataIdx,
                                      uint32_t& imageIdx) const
    {
        return doWriteData(ioWrapper, byteOrder, offset, dataIdx, imageIdx);
    } // TiffComponent::writeData