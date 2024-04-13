    TiffComponent* TiffBinaryArray::doAddChild(TiffComponent::UniquePtr tiffComponent)
    {
        TiffComponent* tc = tiffComponent.release();
        elements_.push_back(tc);
        setDecoded(true);
        return tc;
    } // TiffBinaryArray::doAddChild