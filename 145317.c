    void TiffEntryBase::setValue(Value::UniquePtr value)
    {
        if (value.get() == 0) return;
        tiffType_ = toTiffType(value->typeId());
        count_ = value->count();
        delete pValue_;
        pValue_ = value.release();
    } // TiffEntryBase::setValue