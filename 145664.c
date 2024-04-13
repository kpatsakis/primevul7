    void TiffEncoder::visitBinaryArrayEnd(TiffBinaryArray* object)
    {
        assert(object != 0);

        if (object->cfg() == 0 || !object->decoded()) return;
        std::uint32_t size = object->TiffEntryBase::doSize();
        if (size == 0) return;
        if (!object->initialize(pRoot_)) return;

        // Re-encrypt buffer if necessary
        const CryptFct cryptFct = object->cfg()->cryptFct_;
        if (cryptFct != 0) {
            const byte* pData = object->pData();
            DataBuf buf = cryptFct(object->tag(), pData, size, pRoot_);
            if (buf.size_ > 0) {
                pData = buf.pData_;
                size = static_cast<std::uint32_t>(buf.size_);
            }
            if (!object->updOrigDataBuf(pData, size)) {
                setDirty();
            }
        }
    }