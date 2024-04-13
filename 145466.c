    void OffsetWriter::setOrigin(OffsetId id, uint32_t origin, ByteOrder byteOrder)
    {
        offsetList_[id] = OffsetData(origin, byteOrder);
    }