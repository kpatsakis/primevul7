    size_t MemIo::read(byte* buf, size_t rcount)
    {
        size_t avail = EXV_MAX(p_->size_ - p_->idx_, 0);
        size_t allow = EXV_MIN(rcount, avail);
        if (p_->data_ == nullptr) {
            throw Error(kerCallFailed, "std::memcpy with src == nullptr");
        }
        std::memcpy(buf, &p_->data_[p_->idx_], allow);
        p_->idx_ += allow;
        if (rcount > avail)
            p_->eof_ = true;
        return allow;
    }