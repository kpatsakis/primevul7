    Image::UniquePtr ImageFactory::open(const std::wstring& wpath, bool useCurl)
    {
        Image::UniquePtr image = open(ImageFactory::createIo(wpath, useCurl)); // may throw
        if (image.get() == 0) throw WError(kerFileContainsUnknownImageType, wpath);
        return image;
    }