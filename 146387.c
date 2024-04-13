    Image::UniquePtr newJp2Instance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new Jp2Image(std::move(io), create));
        if (!image->good())
        {
            image.reset();
        }
        return image;
    }