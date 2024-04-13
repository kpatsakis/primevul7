    Loader::UniquePtr createLoaderNative(PreviewId id, const Image &image, int parIdx)
    {
        return Loader::UniquePtr(new LoaderNative(id, image, parIdx));
    }