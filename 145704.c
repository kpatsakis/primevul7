    void IoWrapper::setTarget(int id, uint32_t target)
    {
        if (pow_) pow_->setTarget(OffsetWriter::OffsetId(id), target);
    }