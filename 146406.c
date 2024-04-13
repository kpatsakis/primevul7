    TypeId toTypeId(TiffType tiffType, uint16_t tag, IfdId group)
    {
        TypeId ti = TypeId(tiffType);
        // On the fly type conversion for Exif.Photo.UserComment
        if (tag == 0x9286 && group == exifId && ti == undefined) {
            ti = comment;
        }
        // http://dev.exiv2.org/boards/3/topics/1337 change unsignedByte to signedByte
        // Exif.NikonAFT.AFFineTuneAdj || Exif.Pentax.Temperature
        if ( ti == Exiv2::unsignedByte ) {
            if ( (tag == 0x0002 && group == nikonAFTId ) || (tag == 0x0047 && group == pentaxId) ) {
                ti = Exiv2::signedByte;
            }
        }
        return ti;
    }