    bool LoaderExifDataJpeg::readDimensions()
    {
        if (!valid()) return false;

        DataBuf buf = getData();
        if (buf.size_ == 0) return false;

        try {
            Image::UniquePtr image = ImageFactory::open(buf.pData_, buf.size_);
            if (image.get() == 0) return false;
            image->readMetadata();

            width_ = image->pixelWidth();
            height_ = image->pixelHeight();
        }
        catch (const AnyError& /* error */ ) {
            return false;
        }

        return true;
    }