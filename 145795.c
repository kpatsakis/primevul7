    void TiffEncoder::encodeIptc()
    {
        // Update IPTCNAA Exif tag, if it exists. Delete the tag if there
        // is no IPTC data anymore.
        // If there is new IPTC data and Exif.Image.ImageResources does
        // not exist, create a new IPTCNAA Exif tag.
        bool del = false;
        ExifKey iptcNaaKey("Exif.Image.IPTCNAA");
        ExifData::iterator pos = exifData_.findKey(iptcNaaKey);
        if (pos != exifData_.end()) {
            iptcNaaKey.setIdx(pos->idx());
            exifData_.erase(pos);
            del = true;
        }
        DataBuf rawIptc = IptcParser::encode(iptcData_);
        ExifKey irbKey("Exif.Image.ImageResources");
        pos = exifData_.findKey(irbKey);
        if (pos != exifData_.end()) {
            irbKey.setIdx(pos->idx());
        }
        if (rawIptc.size_ != 0 && (del || pos == exifData_.end())) {
            Value::UniquePtr value = Value::create(unsignedLong);
            DataBuf buf;
            if (rawIptc.size_ % 4 != 0) {
                // Pad the last unsignedLong value with 0s
                buf.alloc((rawIptc.size_ / 4) * 4 + 4);
                memset(buf.pData_, 0x0, buf.size_);
                memcpy(buf.pData_, rawIptc.pData_, rawIptc.size_);
            }
            else {
                buf = rawIptc; // Note: This resets rawIptc
            }
            value->read(buf.pData_, buf.size_, byteOrder_);
            Exifdatum iptcDatum(iptcNaaKey, value.get());
            exifData_.add(iptcDatum);
            pos = exifData_.findKey(irbKey); // needed after add()
        }
        // Also update IPTC IRB in Exif.Image.ImageResources if it exists,
        // but don't create it if not.
        if (pos != exifData_.end()) {
            DataBuf irbBuf(pos->value().size());
            pos->value().copy(irbBuf.pData_, invalidByteOrder);
            irbBuf = Photoshop::setIptcIrb(irbBuf.pData_, irbBuf.size_, iptcData_);
            exifData_.erase(pos);
            if (irbBuf.size_ != 0) {
                Value::UniquePtr value = Value::create(unsignedByte);
                value->read(irbBuf.pData_, irbBuf.size_, invalidByteOrder);
                Exifdatum iptcDatum(irbKey, value.get());
                exifData_.add(iptcDatum);
            }
        }
    } // TiffEncoder::encodeIptc