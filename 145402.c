    uint32_t TiffDataEntry::doWrite(IoWrapper& ioWrapper,
                                    ByteOrder byteOrder,
                                    int32_t   offset,
                                    uint32_t  /*valueIdx*/,
                                    uint32_t  dataIdx,
                                    uint32_t& /*imageIdx*/)
    {
        if (!pValue() || pValue()->count() == 0) return 0;

        DataBuf buf(pValue()->size());
        uint32_t idx = 0;
        const long prevOffset = pValue()->toLong(0);
        for (uint32_t i = 0; i < count(); ++i) {
            const long newDataIdx =   pValue()->toLong(i) - prevOffset
                                    + static_cast<long>(dataIdx);
            idx += writeOffset(buf.pData_ + idx,
                               offset + newDataIdx,
                               tiffType(),
                               byteOrder);
        }
        ioWrapper.write(buf.pData_, buf.size_);
        return (uint32_t)buf.size_;
    } // TiffDataEntry::doWrite