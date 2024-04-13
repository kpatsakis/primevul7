    TiffComponent* TiffSubIfd::doAddChild(TiffComponent::UniquePtr tiffComponent)
    {
        TiffDirectory* d = dynamic_cast<TiffDirectory*>(tiffComponent.release());
        assert(d);
        ifds_.push_back(d);
        return d;
    } // TiffSubIfd::doAddChild