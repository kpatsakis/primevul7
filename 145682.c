    TiffComponent* TiffIfdMakernote::doAddChild(TiffComponent::UniquePtr tiffComponent)
    {
        return ifd_.addChild(std::move(tiffComponent));
    }