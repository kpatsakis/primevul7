    void TiffEncoder::encodeXmp()
    {
#ifdef EXV_HAVE_XMP_TOOLKIT
        ExifKey xmpKey("Exif.Image.XMLPacket");
        // Remove any existing XMP Exif tag
        ExifData::iterator pos = exifData_.findKey(xmpKey);
        if (pos != exifData_.end()) {
            xmpKey.setIdx(pos->idx());
            exifData_.erase(pos);
        }
        std::string xmpPacket;
        if ( xmpData_.usePacket() ) {
            xmpPacket = xmpData_.xmpPacket();
        } else {
            if (XmpParser::encode(xmpPacket, xmpData_) > 1) {
#ifndef SUPPRESS_WARNINGS
                EXV_ERROR << "Failed to encode XMP metadata.\n";
#endif
            }
        }
        if (!xmpPacket.empty()) {
            // Set the XMP Exif tag to the new value
            Value::UniquePtr value = Value::create(unsignedByte);
            value->read(reinterpret_cast<const byte*>(&xmpPacket[0]),
                        static_cast<long>(xmpPacket.size()),
                        invalidByteOrder);
            Exifdatum xmpDatum(xmpKey, value.get());
            exifData_.add(xmpDatum);
        }
#endif
    } // TiffEncoder::encodeXmp