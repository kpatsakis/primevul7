    Image::UniquePtr ImageFactory::create(ImageType type,
                                        const std::wstring& wpath)
    {
        std::unique_ptr<FileIo> fileIo(new FileIo(wpath));
        // Create or overwrite the file, then close it
        if (fileIo->open("w+b") != 0) {
            throw WError(kerFileOpenFailed, wpath, "w+b", strError().c_str());
        }
        fileIo->close();
        BasicIo::UniquePtr io(std::move(fileIo));
        Image::UniquePtr image = create(type, std::move(io));
        if (image.get() == 0) throw Error(kerUnsupportedImageType, static_cast<int>(type));
        return image;
    }