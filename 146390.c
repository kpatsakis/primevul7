    Image::UniquePtr newPgfInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new PgfImage(std::move(io), create));
        if (!image->good())
        {
            image.reset();
        }
        return image;
    }