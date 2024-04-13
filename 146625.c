    JpegBase::JpegBase(ImageType type, BasicIo::UniquePtr io, bool create,
                       const byte initData[], long dataSize)
        : Image(type, mdExif | mdIptc | mdXmp | mdComment, std::move(io))
    {
        if (create) {
            initImage(initData, dataSize);
        }
    }