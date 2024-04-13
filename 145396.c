    int Modify::addMetadatum(Exiv2::Image* pImage, const ModifyCmd& modifyCmd)
    {
        if (Params::instance().verbose_) {
            std::cout << _("Add") << " " << modifyCmd.key_ << " \"" << modifyCmd.value_ << "\" ("
                      << Exiv2::TypeInfo::typeName(modifyCmd.typeId_) << ")" << std::endl;
        }
        Exiv2::ExifData& exifData = pImage->exifData();
        Exiv2::IptcData& iptcData = pImage->iptcData();
        Exiv2::XmpData& xmpData = pImage->xmpData();
        Exiv2::Value::UniquePtr value = Exiv2::Value::create(modifyCmd.typeId_);
        int rc = value->read(modifyCmd.value_);
        if (0 == rc) {
            if (modifyCmd.metadataId_ == exif) {
                exifData.add(Exiv2::ExifKey(modifyCmd.key_), value.get());
            }
            if (modifyCmd.metadataId_ == iptc) {
                iptcData.add(Exiv2::IptcKey(modifyCmd.key_), value.get());
            }
            if (modifyCmd.metadataId_ == xmp) {
                xmpData.add(Exiv2::XmpKey(modifyCmd.key_), value.get());
            }
        } else {
            std::cerr << _("Warning") << ": " << modifyCmd.key_ << ": " << _("Failed to read") << " "
                      << Exiv2::TypeInfo::typeName(value->typeId()) << " " << _("value") << " \"" << modifyCmd.value_
                      << "\"\n";
        }
        return rc;
    }