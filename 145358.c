    int TiffReader::nextIdx(IfdId group)
    {
        return ++idxSeq_[group];
    }