    TiffComponent* TiffComponent::addPath(uint16_t tag,
                                          TiffPath& tiffPath,
                                          TiffComponent* const pRoot,
                                          TiffComponent::UniquePtr object)
    {
        return doAddPath(tag, tiffPath, pRoot, std::move(object));
    } // TiffComponent::addPath