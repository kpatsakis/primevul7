    JpegImage::JpegImage(BasicIo::UniquePtr io, bool create)
        : JpegBase(ImageType::jpeg, std::move(io), create, blank_, sizeof(blank_))
    {
    }