    TiffComponent* TiffIfdMakernote::doAddPath(uint16_t tag,
                                               TiffPath& tiffPath,
                                               TiffComponent* const pRoot,
                                               TiffComponent::UniquePtr object)
    {
        return ifd_.addPath(tag, tiffPath, pRoot, std::move(object));
    }