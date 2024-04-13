    TiffComponent* TiffIfdMakernote::doAddNext(TiffComponent::UniquePtr tiffComponent)
    {
        return ifd_.addNext(std::move(tiffComponent));
    }