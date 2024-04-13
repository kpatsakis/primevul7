    ExvImage::ExvImage(BasicIo::UniquePtr io, bool create)
        : JpegBase(ImageType::exv, std::move(io), create, blank_, sizeof(blank_))
    {
    }