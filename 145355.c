    WebPImage::WebPImage(BasicIo::UniquePtr io)
    : Image(ImageType::webp, mdNone, std::move(io))
    {
    } // WebPImage::WebPImage