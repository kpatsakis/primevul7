    Loader::UniquePtr createLoaderXmpJpeg(PreviewId id, const Image &image, int parIdx)
    {
        return Loader::UniquePtr(new LoaderXmpJpeg(id, image, parIdx));
    }