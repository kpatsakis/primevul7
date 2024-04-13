    Image::Image(ImageType type, uint16_t supportedMetadata, BasicIo::UniquePtr io)
        : io_(std::move(io)),
          pixelWidth_(0),
          pixelHeight_(0),
          imageType_(type),
          supportedMetadata_(supportedMetadata),
#ifdef EXV_HAVE_XMP_TOOLKIT
          writeXmpFromPacket_(false),
#else
          writeXmpFromPacket_(true),
#endif
          byteOrder_(invalidByteOrder),
          tags_(),
          init_(true)
    {
    }