    void TiffEncoder::encodeDataEntry(TiffDataEntry* object, const Exifdatum* datum)
    {
        encodeOffsetEntry(object, datum);

        if (!dirty_ && writeMethod() == wmNonIntrusive) {
            assert(object);
            assert(object->pValue());
            if (  object->sizeDataArea_
                < static_cast<uint32_t>(object->pValue()->sizeDataArea())) {
#ifdef DEBUG
                ExifKey key(object->tag(), groupName(object->group()));
                std::cerr << "DATAAREA GREW     " << key << "\n";
#endif
                setDirty();
            }
            else {
                // Write the new dataarea, fill with 0x0
#ifdef DEBUG
                ExifKey key(object->tag(), groupName(object->group()));
                std::cerr << "Writing data area for " << key << "\n";
#endif
                DataBuf buf = object->pValue()->dataArea();
                memcpy(object->pDataArea_, buf.pData_, buf.size_);
                if (object->sizeDataArea_ > static_cast<uint32_t>(buf.size_)) {
                    memset(object->pDataArea_ + buf.size_,
                           0x0, object->sizeDataArea_ - buf.size_);
                }
            }
        }

    } // TiffEncoder::encodeDataEntry