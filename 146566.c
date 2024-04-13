    TiffComponent* TiffDirectory::doAddNext(TiffComponent::UniquePtr tiffComponent)
    {
        TiffComponent* tc = 0;
        if (hasNext_) {
            tc = tiffComponent.release();
            pNext_ = tc;
        }
        return tc;
    } // TiffDirectory::doAddNext