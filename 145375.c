    TiffComponent* TiffComponent::addChild(TiffComponent::UniquePtr tiffComponent)
    {
        return doAddChild(std::move(tiffComponent));
    } // TiffComponent::addChild