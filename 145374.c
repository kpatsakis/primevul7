    Image::UniquePtr ImageFactory::open(BasicIo::UniquePtr io)
    {
        if (io->open() != 0) {
            throw Error(kerDataSourceOpenFailed, io->path(), strError());
        }
        for (unsigned int i = 0; registry[i].imageType_ != ImageType::none; ++i) {
            if (registry[i].isThisType_(*io, false)) {
                return registry[i].newInstance_(std::move(io), false);
            }
        }
        return Image::UniquePtr();
    }