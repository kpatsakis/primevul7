    uint32_t TiffBinaryElement::doWrite(IoWrapper& ioWrapper,
                                        ByteOrder byteOrder,
                                        int32_t   /*offset*/,
                                        uint32_t  /*valueIdx*/,
                                        uint32_t  /*dataIdx*/,
                                        uint32_t& /*imageIdx*/)
    {
        Value const* pv = pValue();
        if (!pv || pv->count() == 0) return 0;
        DataBuf buf(pv->size());
        pv->copy(buf.pData_, byteOrder);
        ioWrapper.write(buf.pData_, buf.size_);
        return (uint32_t)buf.size_;
    } // TiffBinaryElement::doWrite