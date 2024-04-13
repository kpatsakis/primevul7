    bool TiffBinaryArray::initialize(IfdId group)
    {
        if (arrayCfg_ != 0) return true; // Not a complex array or already initialized

        for (int idx = 0; idx < setSize_; ++idx) {
            if (arraySet_[idx].cfg_.group_ == group) {
                arrayCfg_ = &arraySet_[idx].cfg_;
                arrayDef_ = arraySet_[idx].def_;
                defSize_  = arraySet_[idx].defSize_;
                return true;
            }
        }
        return false;
    }