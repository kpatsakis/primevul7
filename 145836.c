    Image::UniquePtr ImageFactory::create(ImageType type, const std::string& path)
    {
        std::unique_ptr<FileIo> fileIo(new FileIo(path));
        // Create or overwrite the file, then close it
        if (fileIo->open("w+b") != 0) {
            throw Error(kerFileOpenFailed, path, "w+b", strError());
        }
        fileIo->close();
        BasicIo::UniquePtr io(std::move(fileIo));
        Image::UniquePtr image = create(type, std::move(io));
        if (!image)
            throw Error(kerUnsupportedImageType, static_cast<int>(type));
        return image;
    }