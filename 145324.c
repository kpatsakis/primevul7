    Image::UniquePtr newCrwInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new CrwImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }