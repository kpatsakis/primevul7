    MrwImage::MrwImage(BasicIo::UniquePtr io, bool /*create*/)
        : Image(ImageType::mrw, mdExif | mdIptc | mdXmp, std::move(io))
    {
    } // MrwImage::MrwImage