    void Converter::syncExifWithXmp()
    {
        Exiv2::XmpData::iterator td = xmpData_->findKey(XmpKey("Xmp.tiff.NativeDigest"));
        Exiv2::XmpData::iterator ed = xmpData_->findKey(XmpKey("Xmp.exif.NativeDigest"));
        if (td != xmpData_->end() && ed != xmpData_->end()) {
            if (td->value().toString() == computeExifDigest(true) &&
                ed->value().toString() == computeExifDigest(false)) {
                // We have both digests and the values match
                // XMP is up-to-date, we should update Exif
                setOverwrite(true);
                setErase(false);

                cnvFromXmp();
                writeExifDigest();
                return;
            }
            else {
                // We have both digests and the values do not match
                // Exif was modified after XMP, we should update XMP
                setOverwrite(true);
                setErase(false);

                cnvToXmp();
                writeExifDigest();
                return;
            }
        }
        else {
            // We don't have both digests, it is probably the first conversion to XMP
            setOverwrite(false); // to be safe
            setErase(false);

            cnvToXmp();
            writeExifDigest();
            return;
        }
    }