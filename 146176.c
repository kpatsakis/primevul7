    Thumbnail::UniquePtr Thumbnail::create(const Exiv2::ExifData& exifData)
    {
        Thumbnail::UniquePtr thumbnail;
        const Exiv2::ExifKey k1("Exif.Thumbnail.Compression");
        Exiv2::ExifData::const_iterator pos = exifData.findKey(k1);
        if (pos != exifData.end()) {
            if (pos->count() == 0) return thumbnail;
            long compression = pos->toLong();
            if (compression == 6) {
                thumbnail = Thumbnail::UniquePtr(new JpegThumbnail);
            }
            else {
                thumbnail = Thumbnail::UniquePtr(new TiffThumbnail);
            }
        }
        else {
            const Exiv2::ExifKey k2("Exif.Thumbnail.JPEGInterchangeFormat");
            pos = exifData.findKey(k2);
            if (pos != exifData.end()) {
                thumbnail = Thumbnail::UniquePtr(new JpegThumbnail);
            }
        }
        return thumbnail;
    }