    Image::UniquePtr newJpegInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new JpegImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }