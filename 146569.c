    Converter::Converter(ExifData& exifData, XmpData& xmpData)
        : erase_(false), overwrite_(true), exifData_(&exifData), iptcData_(0), xmpData_(&xmpData), iptcCharset_(0)
    {
    }