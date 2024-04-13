    Image::UniquePtr newExvInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image;
        image = Image::UniquePtr(new ExvImage(std::move(io), create));
        if (!image->good()) image.reset();
        return image;
    }