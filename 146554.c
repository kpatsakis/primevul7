    PreviewProperties LoaderXmpJpeg::getProperties() const
    {
        PreviewProperties prop = Loader::getProperties();
        prop.mimeType_ = "image/jpeg";
        prop.extension_ = ".jpg";
#ifdef EXV_UNICODE_PATH
        prop.wextension_ = EXV_WIDEN(".jpg");
#endif
        return prop;
    }