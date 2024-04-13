    Loader::UniquePtr createLoaderExifJpeg(PreviewId id, const Image &image, int parIdx)
    {
        return Loader::UniquePtr(new LoaderExifJpeg(id, image, parIdx));
    }