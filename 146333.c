    Image::UniquePtr ImageFactory::open(const byte* data, size_t size)
    {
        BasicIo::UniquePtr io(new MemIo(data, size));
        Image::UniquePtr image = open(std::move(io)); // may throw
        if (image.get() == 0) throw Error(kerMemoryContainsUnknownImageType);
        return image;
    }