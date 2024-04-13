    void Modify::delMetadatum(Exiv2::Image* pImage, const ModifyCmd& modifyCmd)
    {
        if (Params::instance().verbose_) {
            std::cout << _("Del") << " " << modifyCmd.key_ << std::endl;
        }

        Exiv2::ExifData& exifData = pImage->exifData();
        Exiv2::IptcData& iptcData = pImage->iptcData();
        Exiv2::XmpData& xmpData = pImage->xmpData();
        if (modifyCmd.metadataId_ == exif) {
            Exiv2::ExifData::iterator pos;
            Exiv2::ExifKey exifKey = Exiv2::ExifKey(modifyCmd.key_);
            while ((pos = exifData.findKey(exifKey)) != exifData.end()) {
                exifData.erase(pos);
            }
        }
        if (modifyCmd.metadataId_ == iptc) {
            Exiv2::IptcData::iterator pos;
            Exiv2::IptcKey iptcKey = Exiv2::IptcKey(modifyCmd.key_);
            while ((pos = iptcData.findKey(iptcKey)) != iptcData.end()) {
                iptcData.erase(pos);
            }
        }
        if (modifyCmd.metadataId_ == xmp) {
            Exiv2::XmpData::iterator pos;
            Exiv2::XmpKey xmpKey = Exiv2::XmpKey(modifyCmd.key_);
            if ((pos = xmpData.findKey(xmpKey)) != xmpData.end()) {
                xmpData.eraseFamily(pos);
            }
        }
    }