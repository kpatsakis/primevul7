    bool LoaderNative::readDimensions()
    {
        if (!valid()) return false;
        if (width_ != 0 || height_ != 0) return true;

        const DataBuf data = getData();
        if (data.size_ == 0) return false;
        try {
            Image::UniquePtr image = ImageFactory::open(data.pData_, data.size_);
            if (image.get() == 0) return false;
            image->readMetadata();

            width_ = image->pixelWidth();
            height_ = image->pixelHeight();
        } catch (const AnyError& /* error */) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Invalid native preview image.\n";
#endif
            return false;
        }
        return true;
    }