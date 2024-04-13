    RafImage::RafImage(BasicIo::UniquePtr io, bool /*create*/)
        : Image(ImageType::raf, mdExif | mdIptc | mdXmp, std::move(io))
    {
    } // RafImage::RafImage