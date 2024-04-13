    Image::UniquePtr newTiffInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new TiffImage(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }