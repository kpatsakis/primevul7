    int Modify::setMetadatum(Exiv2::Image* pImage, const ModifyCmd& modifyCmd)
    {
        if (Params::instance().verbose_) {
            std::cout << _("Set") << " " << modifyCmd.key_ << " \"" << modifyCmd.value_ << "\" ("
                      << Exiv2::TypeInfo::typeName(modifyCmd.typeId_) << ")" << std::endl;
        }
        Exiv2::ExifData& exifData = pImage->exifData();
        Exiv2::IptcData& iptcData = pImage->iptcData();
        Exiv2::XmpData& xmpData = pImage->xmpData();
        Exiv2::Metadatum* metadatum = 0;
        if (modifyCmd.metadataId_ == exif) {
            auto pos = exifData.findKey(Exiv2::ExifKey(modifyCmd.key_));
            if (pos != exifData.end()) {
                metadatum = &(*pos);
            }
        }
        if (modifyCmd.metadataId_ == iptc) {
            auto pos = iptcData.findKey(Exiv2::IptcKey(modifyCmd.key_));
            if (pos != iptcData.end()) {
                metadatum = &(*pos);
            }
        }
        if (modifyCmd.metadataId_ == xmp) {
            auto pos = xmpData.findKey(Exiv2::XmpKey(modifyCmd.key_));
            if (pos != xmpData.end()) {
                metadatum = &(*pos);
            }
        }
        // If a type was explicitly requested, use it; else
        // use the current type of the metadatum, if any;
        // or the default type
        Exiv2::Value::UniquePtr value;
        if (metadatum) {
            value = metadatum->getValue();
        }
        if (value.get() == 0 || (modifyCmd.explicitType_ && modifyCmd.typeId_ != value->typeId())) {
            value = Exiv2::Value::create(modifyCmd.typeId_);
        }
        int rc = value->read(modifyCmd.value_);
        if (0 == rc) {
            if (metadatum) {
                metadatum->setValue(value.get());
            } else {
                if (modifyCmd.metadataId_ == exif) {
                    exifData.add(Exiv2::ExifKey(modifyCmd.key_), value.get());
                }
                if (modifyCmd.metadataId_ == iptc) {
                    iptcData.add(Exiv2::IptcKey(modifyCmd.key_), value.get());
                }
                if (modifyCmd.metadataId_ == xmp) {
                    xmpData.add(Exiv2::XmpKey(modifyCmd.key_), value.get());
                }
            }
        } else {
            std::cerr << _("Warning") << ": " << modifyCmd.key_ << ": " << _("Failed to read") << " "
                      << Exiv2::TypeInfo::typeName(value->typeId()) << " " << _("value") << " \"" << modifyCmd.value_
                      << "\"\n";
        }
        return rc;
    }