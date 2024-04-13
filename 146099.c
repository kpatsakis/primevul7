    void TiffEncoder::encodeImageEntry(TiffImageEntry* object, const Exifdatum* datum)
    {
        encodeOffsetEntry(object, datum);

        uint32_t sizeDataArea = object->pValue()->sizeDataArea();

        if (sizeDataArea > 0 && writeMethod() == wmNonIntrusive) {
#ifdef DEBUG
            std::cerr << "\t DATAAREA IS SET (NON-INTRUSIVE WRITING)";
#endif
            setDirty();
        }

        if (sizeDataArea > 0 && writeMethod() == wmIntrusive) {
#ifdef DEBUG
            std::cerr << "\t DATAAREA IS SET (INTRUSIVE WRITING)";
#endif
            // Set pseudo strips (without a data pointer) from the size tag
            ExifKey key(object->szTag(), groupName(object->szGroup()));
            ExifData::const_iterator pos = exifData_.findKey(key);
            const byte* zero = 0;
            if (pos == exifData_.end()) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Size tag " << key
                          << " not found. Writing only one strip.\n";
#endif
                object->strips_.clear();
                object->strips_.push_back(std::make_pair(zero, sizeDataArea));
            }
            else {
                uint32_t sizeTotal = 0;
                object->strips_.clear();
                for (long i = 0; i < pos->count(); ++i) {
                    uint32_t len = pos->toLong(i);
                    object->strips_.push_back(std::make_pair(zero, len));
                    sizeTotal += len;
                }
                if (sizeTotal != sizeDataArea) {
#ifndef SUPPRESS_WARNINGS
                    ExifKey key2(object->tag(), groupName(object->group()));
                    EXV_ERROR << "Sum of all sizes of " << key
                              << " != data size of " << key2 << ". "
                              << "This results in an invalid image.\n";
#endif
                    // Todo: How to fix? Write only one strip?
                }
            }
        }

        if (sizeDataArea == 0 && writeMethod() == wmIntrusive) {
#ifdef DEBUG
            std::cerr << "\t USE STRIPS FROM SOURCE TREE IMAGE ENTRY";
#endif
            // Set strips from source tree
            if (pSourceTree_) {
                TiffFinder finder(object->tag(), object->group());
                pSourceTree_->accept(finder);
                TiffImageEntry* ti = dynamic_cast<TiffImageEntry*>(finder.result());
                if (ti) {
                    object->strips_ = ti->strips_;
                }
            }
#ifndef SUPPRESS_WARNINGS
            else {
                ExifKey key2(object->tag(), groupName(object->group()));
                EXV_WARNING << "No image data to encode " << key2 << ".\n";
            }
#endif
        }

    } // TiffEncoder::encodeImageEntry