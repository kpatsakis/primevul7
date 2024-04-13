    PreviewProperties LoaderNative::getProperties() const
    {
        PreviewProperties prop = Loader::getProperties();
        prop.mimeType_ = nativePreview_.mimeType_;
        if (nativePreview_.mimeType_ == "image/jpeg") {
            prop.extension_ = ".jpg";
        } else if (nativePreview_.mimeType_ == "image/tiff") {
            prop.extension_ = ".tif";
        } else if (nativePreview_.mimeType_ == "image/x-wmf") {
            prop.extension_ = ".wmf";
        } else if (nativePreview_.mimeType_ == "image/x-portable-anymap") {
            prop.extension_ = ".pnm";
        } else {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Unknown native preview format: " << nativePreview_.mimeType_ << "\n";
#endif
            prop.extension_ = ".dat";
        }
#ifdef EXV_UNICODE_PATH
        prop.wextension_ = s2ws(prop.extension_);
#endif
        return prop;
    }