    PsdImage::PsdImage(BasicIo::UniquePtr io)
        : Image(ImageType::psd, mdExif | mdIptc | mdXmp, std::move(io))
    {
    }  // PsdImage::PsdImage