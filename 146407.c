    void TiffReader::visitBinaryElement(TiffBinaryElement* object)
    {
        byte* pData   = object->start();
        uint32_t size = object->TiffEntryBase::doSize();
        ByteOrder bo = object->elByteOrder();
        if (bo == invalidByteOrder) bo = byteOrder();
        TypeId typeId = toTypeId(object->elDef()->tiffType_, object->tag(), object->group());
        Value::UniquePtr v = Value::create(typeId);
        enforce(v.get() != nullptr, kerCorruptedMetadata);
        v->read(pData, size, bo);

        object->setValue(std::move(v));
        object->setOffset(0);
        object->setIdx(nextIdx(object->group()));

    } // TiffReader::visitBinaryElement