    Image::UniquePtr ImageFactory::create(ImageType type)
    {
        BasicIo::UniquePtr io(new MemIo);
        Image::UniquePtr image = create(type, std::move(io));
        if (image.get() == 0) {
            throw Error(kerUnsupportedImageType, static_cast<int>(type));
        }
        return image;
    }