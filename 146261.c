    void OffsetWriter::setTarget(OffsetId id, uint32_t target)
    {
        OffsetList::iterator it = offsetList_.find(id);
        if (it != offsetList_.end()) it->second.target_ = target;
    }