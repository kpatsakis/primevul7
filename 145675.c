    Image::UniquePtr newBigTiffInstance(BasicIo::UniquePtr io, bool)
    {
        return Image::UniquePtr(new BigTiffImage(std::move(io)));
    }