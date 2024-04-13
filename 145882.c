    TiffComponent* TiffMnEntry::doAddNext(TiffComponent::UniquePtr tiffComponent)
    {
        TiffComponent* tc = 0;
        if (mn_) {
            tc = mn_->addNext(std::move(tiffComponent));
        }
        return tc;
    } // TiffMnEntry::doAddNext