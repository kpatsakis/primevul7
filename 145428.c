    TiffComponent* TiffDirectory::doAddChild(TiffComponent::UniquePtr tiffComponent)
    {
        TiffComponent* tc = tiffComponent.release();
        components_.push_back(tc);
        return tc;
    } // TiffDirectory::doAddChild