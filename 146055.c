    void Converter::cnvFromXmp()
    {
        for (unsigned int i = 0; i < EXV_COUNTOF(conversion_); ++i) {
            const Conversion& c = conversion_[i];
            if (   (c.metadataId_ == mdExif && exifData_)
                || (c.metadataId_ == mdIptc && iptcData_)) {
                EXV_CALL_MEMBER_FN(*this, c.key2ToKey1_)(c.key2_, c.key1_);
            }
        }
    }