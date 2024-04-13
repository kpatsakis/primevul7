    int Print::printMetadata(const Exiv2::Image* image)
    {
        bool ret = false;
        bool noExif = false;
        if (Params::instance().printTags_ & Exiv2::mdExif) {
            const Exiv2::ExifData& exifData = image->exifData();
            for (auto md = exifData.begin(); md != exifData.end(); ++md) {
                ret |= printMetadatum(*md, image);
            }
            if (exifData.empty())
                noExif = true;
        }

        bool noIptc = false;
        if (Params::instance().printTags_ & Exiv2::mdIptc) {
            const Exiv2::IptcData& iptcData = image->iptcData();
            for (auto md = iptcData.begin(); md != iptcData.end(); ++md) {
                ret |= printMetadatum(*md, image);
            }
            if (iptcData.empty())
                noIptc = true;
        }

        bool noXmp = false;
        if (Params::instance().printTags_ & Exiv2::mdXmp) {
            const Exiv2::XmpData& xmpData = image->xmpData();
            for (auto md = xmpData.begin(); md != xmpData.end(); ++md) {
                ret |= printMetadatum(*md, image);
            }
            if (xmpData.empty())
                noXmp = true;
        }

        // With -v, inform about the absence of any (requested) type of metadata
        if (Params::instance().verbose_) {
            if (noExif)
                std::cerr << path_ << ": " << _("No Exif data found in the file\n");
            if (noIptc)
                std::cerr << path_ << ": " << _("No IPTC data found in the file\n");
            if (noXmp)
                std::cerr << path_ << ": " << _("No XMP data found in the file\n");
        }

        // With -g or -K, return -3 if no matching tags were found
        int rc = 0;
        if ((!Params::instance().greps_.empty() || !Params::instance().keys_.empty()) && !ret)
            rc = 1;

        return rc;
    }