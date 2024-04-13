    PreviewImage PreviewManager::getPreviewImage(const PreviewProperties &properties) const
    {
        Loader::UniquePtr loader = Loader::create(properties.id_, image_);
        DataBuf buf;
        if (loader.get()) {
            buf = loader->getData();
        }

        return PreviewImage(properties, buf);
    }