    uint32_t TiffDirectory::doWriteData(IoWrapper& ioWrapper,
                                        ByteOrder byteOrder,
                                        int32_t   offset,
                                        uint32_t  dataIdx,
                                        uint32_t& imageIdx) const
    {
        uint32_t len = 0;
        for (Components::const_iterator i = components_.begin(); i != components_.end(); ++i) {
            len += (*i)->writeData(ioWrapper, byteOrder, offset, dataIdx + len, imageIdx);
        }
        return len;
    } // TiffDirectory::doWriteData