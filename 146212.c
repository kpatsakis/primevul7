    TiffComponent* TiffComponent::addNext(TiffComponent::UniquePtr tiffComponent)
    {
        return doAddNext(std::move(tiffComponent));
    } // TiffComponent::addNext