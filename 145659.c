    uint32_t TiffMnEntry::doWrite(IoWrapper& ioWrapper,
                                  ByteOrder byteOrder,
                                  int32_t   offset,
                                  uint32_t  valueIdx,
                                  uint32_t  dataIdx,
                                  uint32_t& imageIdx)
    {
        if (!mn_) {
            return TiffEntryBase::doWrite(ioWrapper, byteOrder, offset, valueIdx, dataIdx, imageIdx);
        }
        return mn_->write(ioWrapper, byteOrder, offset + valueIdx, uint32_t(-1), uint32_t(-1), imageIdx);
    } // TiffMnEntry::doWrite