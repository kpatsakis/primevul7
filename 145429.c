    PreviewPropertiesList PreviewManager::getPreviewProperties() const
    {
        PreviewPropertiesList list;
        // go through the loader table and store all successfully created loaders in the list
        for (PreviewId id = 0; id < Loader::getNumLoaders(); ++id) {
            Loader::UniquePtr loader = Loader::create(id, image_);
            if (loader.get() && loader->readDimensions()) {
                PreviewProperties props = loader->getProperties();
                DataBuf buf             = loader->getData(); // #16 getPreviewImage()
                props.size_             = (uint32_t)buf.size_;         //     update the size
                list.push_back(props) ;
            }
        }
        std::sort(list.begin(), list.end(), cmpPreviewProperties);
        return list;
    }