    void syncExifWithXmp(ExifData& exifData, XmpData& xmpData)
    {
        Converter converter(exifData, xmpData);
        converter.syncExifWithXmp();
    }