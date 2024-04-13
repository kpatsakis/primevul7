    bool TiffBinaryArray::initialize(TiffComponent* const pRoot)
    {
        if (cfgSelFct_ == 0) return true; // Not a complex array

        int idx = cfgSelFct_(tag(), pData(), TiffEntryBase::doSize(), pRoot);
        if (idx > -1) {
            arrayCfg_ = &arraySet_[idx].cfg_;
            arrayDef_ = arraySet_[idx].def_;
            defSize_  = arraySet_[idx].defSize_;
        }
        return idx > -1;
    }