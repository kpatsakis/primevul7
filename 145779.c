    bool LoaderExifJpeg::readDimensions()
    {
        if (!valid()) return false;
        if (width_ || height_) return true;

        BasicIo &io = image_.io();

        if (io.open() != 0) {
            throw Error(kerDataSourceOpenFailed, io.path(), strError());
        }
        IoCloser closer(io);
        const Exiv2::byte* base = io.mmap();

        try {
            Image::UniquePtr image = ImageFactory::open(base + offset_, size_);
            if (image.get() == 0) return false;
            image->readMetadata();

            width_ = image->pixelWidth();
            height_ = image->pixelHeight();
        }
        catch (const AnyError& /* error */ ) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Invalid JPEG preview image.\n";
#endif
            return false;
        }

        return true;
    }