    TiffComponent* TiffMnEntry::doAddChild(TiffComponent::UniquePtr tiffComponent)
    {
        TiffComponent* tc = 0;
        if (mn_) {
            tc =  mn_->addChild(std::move(tiffComponent));
        }
        return tc;
    } // TiffMnEntry::doAddChild