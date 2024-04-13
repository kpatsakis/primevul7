    Exiv2::DataBuf TiffThumbnail::copy(const Exiv2::ExifData& exifData) const
    {
        Exiv2::ExifData thumb;
        // Copy all Thumbnail (IFD1) tags from exifData to Image (IFD0) tags in thumb
        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i) {
            if (i->groupName() == "Thumbnail") {
                std::string key = "Exif.Image." + i->tagName();
                thumb.add(Exiv2::ExifKey(key), &i->value());
            }
        }

        Exiv2::MemIo io;
        Exiv2::IptcData emptyIptc;
        Exiv2::XmpData  emptyXmp;
        Exiv2::TiffParser::encode(io, 0, 0, Exiv2::littleEndian, thumb, emptyIptc, emptyXmp);
        return io.read((long) io.size());
    }