    Image::UniquePtr newXmpInstance(BasicIo::UniquePtr io, bool create)
    {
        Image::UniquePtr image(new XmpSidecar(std::move(io), create));
        if (!image->good()) {
            image.reset();
        }
        return image;
    }