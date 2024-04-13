    Image::UniquePtr newRafInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new RafImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }