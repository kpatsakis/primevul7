    PreviewProperties LoaderTiff::getProperties() const
    {
        PreviewProperties prop = Loader::getProperties();
        prop.mimeType_ = "image/tiff";
        prop.extension_ = ".tif";
#ifdef EXV_UNICODE_PATH
        prop.wextension_ = EXV_WIDEN(".tif");
#endif
        return prop;
    }