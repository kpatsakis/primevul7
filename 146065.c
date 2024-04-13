    TiffImage::TiffImage(BasicIo::UniquePtr io, bool /*create*/)
        : Image(ImageType::tiff, mdExif | mdIptc | mdXmp, std::move(io)),
          pixelWidth_(0), pixelHeight_(0)
    {
    } // TiffImage::TiffImage