    TiffComponent::UniquePtr TiffComponent::clone() const
    {
        return UniquePtr(doClone());
    }