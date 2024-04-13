    void TiffEncoder::encodeTiffComponent(
              TiffEntryBase* object,
        const Exifdatum*     datum
    )
    {
        assert(object != 0);

        ExifData::iterator pos = exifData_.end();
        const Exifdatum* ed = datum;
        if (ed == 0) {
            // Non-intrusive writing: find matching tag
            ExifKey key(object->tag(), groupName(object->group()));
            pos = exifData_.findKey(key);
            if (pos != exifData_.end()) {
                ed = &(*pos);
                if (object->idx() != pos->idx()) {
                    // Try to find exact match (in case of duplicate tags)
                    ExifData::iterator pos2 =
                        std::find_if(exifData_.begin(), exifData_.end(),
                                     FindExifdatum2(object->group(), object->idx()));
                    if (pos2 != exifData_.end() && pos2->key() == key.key()) {
                        ed = &(*pos2);
                        pos = pos2; // make sure we delete the correct tag below
                    }
                }
            }
            else {
                setDirty();
#ifdef DEBUG
                std::cerr << "DELETING          " << key << ", idx = " << object->idx() << "\n";
#endif
            }
        }
        else {
            // For intrusive writing, the index is used to preserve the order of
            // duplicate tags
            object->idx_ = ed->idx();
        }
        // Skip encoding image tags of existing TIFF image - they were copied earlier -
        // but encode image tags of new images (creation)
        if (ed && !isImageTag(object->tag(), object->group())) {
            const EncoderFct fct = findEncoderFct_(make_, object->tag(), object->group());
            if (fct) {
                // If an encoding function is registered for the tag, use it
                EXV_CALL_MEMBER_FN(*this, fct)(object, ed);
            }
            else {
                // Else use the encode function at the object (results in a double-dispatch
                // to the appropriate encoding function of the encoder.
                object->encode(*this, ed);
            }
        }
        if (del_ && pos != exifData_.end()) {
            exifData_.erase(pos);
        }
#ifdef DEBUG
        std::cerr << "\n";
#endif
    } // TiffEncoder::encodeTiffComponent