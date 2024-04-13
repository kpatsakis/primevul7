    void moveExifToXmp(ExifData& exifData, XmpData& xmpData)
    {
        Converter converter(exifData, xmpData);
        converter.setErase();
        converter.cnvToXmp();
    }