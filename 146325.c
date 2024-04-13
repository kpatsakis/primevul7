    void Converter::writeExifDigest()
    {
#ifdef EXV_HAVE_XMP_TOOLKIT
        (*xmpData_)["Xmp.tiff.NativeDigest"] = computeExifDigest(true);
        (*xmpData_)["Xmp.exif.NativeDigest"] = computeExifDigest(false);
#endif
    }