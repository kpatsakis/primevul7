    std::string Converter::computeExifDigest(bool tiff)
    {
        std::ostringstream res;
        MD5_CTX    context;
        unsigned char digest[16];

        MD5Init ( &context );
        for (unsigned int i = 0; i < EXV_COUNTOF(conversion_); ++i) {
            const Conversion& c = conversion_[i];
            if (c.metadataId_ == mdExif) {
                Exiv2::ExifKey key(c.key1_);
                if (tiff && key.groupName() != "Image") continue;
                if (!tiff && key.groupName() == "Image") continue;

                if (!res.str().empty()) res << ',';
                res << key.tag();
                Exiv2::ExifData::iterator pos = exifData_->findKey(key);
                if (pos == exifData_->end()) continue;
                DataBuf data(pos->size());
                pos->copy(data.pData_, littleEndian /* FIXME ? */);
                MD5Update ( &context, data.pData_, (uint32_t)data.size_);
            }
        }
        MD5Final(digest, &context);
        res << ';';
        res << std::setw(2) << std::setfill('0') << std::hex << std::uppercase;
        for (int i = 0; i < 16; ++i) {
            res << static_cast<int>(digest[i]);
        }
        return res.str();
    }