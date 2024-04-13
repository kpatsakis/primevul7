    void OffsetWriter::writeOffsets(BasicIo& io) const
    {
        for (OffsetList::const_iterator it = offsetList_.begin(); it != offsetList_.end(); ++it) {
            io.seek(it->second.origin_, BasicIo::beg);
            byte buf[4] = { 0, 0, 0, 0 };
            l2Data(buf, it->second.target_, it->second.byteOrder_);
            io.write(buf, 4);
        }
    }