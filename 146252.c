    Loader::UniquePtr createLoaderExifDataJpeg(PreviewId id, const Image &image, int parIdx)
    {
        return Loader::UniquePtr(new LoaderExifDataJpeg(id, image, parIdx));
    }