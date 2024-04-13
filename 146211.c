    Loader::UniquePtr createLoaderTiff(PreviewId id, const Image &image, int parIdx)
    {
        return Loader::UniquePtr(new LoaderTiff(id, image, parIdx));
    }