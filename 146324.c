    void TiffEntryBase::updateValue(Value::UniquePtr value, ByteOrder byteOrder)
    {
        if (value.get() == 0)
            return;
        uint32_t newSize = value->size();
        if (newSize > size_) {
            setData(DataBuf(newSize));
        }
        if (pData_ != nullptr) {
            memset(pData_, 0x0, size_);
        }
        size_ = value->copy(pData_, byteOrder);
        assert(size_ == newSize);
        setValue(std::move(value));
    } // TiffEntryBase::updateValue