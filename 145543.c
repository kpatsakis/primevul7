    CrwImage::CrwImage(BasicIo::UniquePtr io, bool /*create*/)
        : Image(ImageType::crw, mdExif | mdComment, std::move(io))
    {
    } // CrwImage::CrwImage