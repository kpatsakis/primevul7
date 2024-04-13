    uint32_t TiffBinaryArray::doSize() const
    {
        if (cfg() == 0 || !decoded()) return TiffEntryBase::doSize();

        if (elements_.empty()) return 0;

        // Remaining assumptions:
        // - array elements don't "overlap"
        // - no duplicate tags in the array
        uint32_t idx = 0;
        uint32_t sz = cfg()->tagStep();
        for (Components::const_iterator i = elements_.begin(); i != elements_.end(); ++i) {
            if ((*i)->tag() > idx) {
                idx = (*i)->tag();
                sz = (*i)->size();
            }
        }
        idx = idx * cfg()->tagStep() + sz;

        if (cfg()->hasFillers_ && def()) {
            const ArrayDef* lastDef = def() + defSize() - 1;
            uint16_t lastTag = static_cast<uint16_t>(lastDef->idx_ / cfg()->tagStep());
            idx = EXV_MAX(idx, lastDef->idx_ + lastDef->size(lastTag, cfg()->group_));
        }
        return idx;

    } // TiffBinaryArray::doSize