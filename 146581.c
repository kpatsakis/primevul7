    Image::UniquePtr newMrwInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new MrwImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }