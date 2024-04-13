    uint32_t TiffBinaryArray::doWrite(IoWrapper& ioWrapper,
                                      ByteOrder byteOrder,
                                      int32_t   offset,
                                      uint32_t  valueIdx,
                                      uint32_t  dataIdx,
                                      uint32_t& imageIdx)
    {
        if (cfg() == 0 || !decoded()) return TiffEntryBase::doWrite(ioWrapper,
                                                                    byteOrder,
                                                                    offset,
                                                                    valueIdx,
                                                                    dataIdx,
                                                                    imageIdx);
        if (cfg()->byteOrder_ != invalidByteOrder) byteOrder = cfg()->byteOrder_;
        // Tags must be sorted in ascending order
        std::sort(elements_.begin(), elements_.end(), cmpTagLt);
        uint32_t idx = 0;
        MemIo mio;
        IoWrapper mioWrapper(mio, 0, 0, 0);
        // Some array entries need to have the size in the first element
        if (cfg()->hasSize_) {
            byte buf[4];
            long elSize = TypeInfo::typeSize(toTypeId(cfg()->elTiffType_, 0, cfg()->group_));
            switch (elSize) {
            case 2:
                idx += us2Data(buf, size(), byteOrder);
                break;
            case 4:
                idx += ul2Data(buf, size(), byteOrder);
                break;
            default:
                assert(false);
            }
            mioWrapper.write(buf, elSize);
        }
        // write all tags of the array (Todo: assumes that there are no duplicates, need check)
        for (Components::const_iterator i = elements_.begin(); i != elements_.end(); ++i) {
            // Skip the manufactured tag, if it exists
            if (cfg()->hasSize_ && (*i)->tag() == 0) continue;
            uint32_t newIdx = (*i)->tag() * cfg()->tagStep();
            idx += fillGap(mioWrapper, idx, newIdx);
            idx += (*i)->write(mioWrapper, byteOrder, offset + newIdx, valueIdx, dataIdx, imageIdx);
        }
        if (cfg()->hasFillers_ && def()) {
            const ArrayDef* lastDef = def() + defSize() - 1;
            uint16_t lastTag = static_cast<uint16_t>(lastDef->idx_ / cfg()->tagStep());
            idx += fillGap(mioWrapper, idx, lastDef->idx_ + lastDef->size(lastTag, cfg()->group_));
        }
        DataBuf buf;
        if (cfg()->cryptFct_) {
            buf = cfg()->cryptFct_(tag(), mio.mmap(), static_cast<uint32_t>(mio.size()), pRoot_);
        }
        if (buf.size_ > 0) {
            ioWrapper.write(buf.pData_, buf.size_);
        }
        else {
            ioWrapper.write(mio.mmap(), static_cast<uint32_t>(mio.size()));
        }

        return idx;
    } // TiffBinaryArray::doWrite