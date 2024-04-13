    void TiffEncoder::encodeOffsetEntry(TiffEntryBase* object, const Exifdatum* datum)
    {
        assert(object != 0);
        assert(datum != 0);

        size_t newSize = datum->size();
        if (newSize > object->size_) { // value doesn't fit, encode for intrusive writing
            setDirty();
            object->updateValue(datum->getValue(), byteOrder()); // clones the value
#ifdef DEBUG
            ExifKey key(object->tag(), groupName(object->group()));
            std::cerr << "UPDATING DATA     " << key;
            std::cerr << "\t\t\t ALLOCATED " << object->size() << " BYTES";
#endif
        }
        else {
            object->setValue(datum->getValue()); // clones the value
#ifdef DEBUG
            ExifKey key(object->tag(), groupName(object->group()));
            std::cerr << "NOT UPDATING      " << key;
            std::cerr << "\t\t\t PRESERVE VALUE DATA";
#endif
        }

    } // TiffEncoder::encodeOffsetEntry