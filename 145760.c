    uint32_t TiffBinaryArray::addElement(uint32_t idx, const ArrayDef& def)
    {
        uint16_t tag = static_cast<uint16_t>(idx / cfg()->tagStep());
        int32_t sz = EXV_MIN(def.size(tag, cfg()->group_), TiffEntryBase::doSize() - idx);
        TiffComponent::UniquePtr tc = TiffCreator::create(tag, cfg()->group_);
        TiffBinaryElement* tp = dynamic_cast<TiffBinaryElement*>(tc.get());
        // The assertion typically fails if a component is not configured in
        // the TIFF structure table (TiffCreator::tiffTreeStruct_)
        assert(tp);
        tp->setStart(pData() + idx);
        tp->setData(const_cast<byte*>(pData() + idx), sz);
        tp->setElDef(def);
        tp->setElByteOrder(cfg()->byteOrder_);
        addChild(std::move(tc));
        return sz;
    } // TiffBinaryArray::addElement