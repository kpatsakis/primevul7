    void TiffEncoder::visitMnEntry(TiffMnEntry* object)
    {
        // Test is required here as well as in the callback encoder function
        if (!object->mn_) {
            encodeTiffComponent(object);
        }
        else if (del_) {
            // The makernote is made up of decoded tags, delete binary tag
            ExifKey key(object->tag(), groupName(object->group()));
            ExifData::iterator pos = exifData_.findKey(key);
            if (pos != exifData_.end()) exifData_.erase(pos);
        }
    }