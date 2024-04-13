    uint32_t TiffEntryBase::doWrite(IoWrapper& ioWrapper,
                                    ByteOrder byteOrder,
                                    int32_t   /*offset*/,
                                    uint32_t  /*valueIdx*/,
                                    uint32_t  /*dataIdx*/,
                                    uint32_t& /*imageIdx*/)
    {
        if (!pValue_) return 0;

        DataBuf buf(pValue_->size());
        pValue_->copy(buf.pData_, byteOrder);
        ioWrapper.write(buf.pData_, buf.size_);
        return (uint32_t)buf.size_;
    } // TiffEntryBase::doWrite