    void moveXmpToExif(XmpData& xmpData, ExifData& exifData)
    {
        Converter converter(exifData, xmpData);
        converter.setErase();
        converter.cnvFromXmp();
    }