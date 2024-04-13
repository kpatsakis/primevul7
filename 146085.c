    TiffComponent* TiffMnEntry::doAddPath(uint16_t tag,
                                          TiffPath& tiffPath,
                                          TiffComponent* const pRoot,
                                          TiffComponent::UniquePtr object)
    {
        assert(!tiffPath.empty());
        const TiffPathItem tpi1 = tiffPath.top();
        tiffPath.pop();
        if (tiffPath.empty()) {
            // If the last element in the path is the makernote tag itself we're done
            return this;
        }
        const TiffPathItem tpi2 = tiffPath.top();
        tiffPath.push(tpi1);
        if (mn_ == 0) {
            mnGroup_ = tpi2.group();
            mn_ = TiffMnCreator::create(tpi1.tag(), tpi1.group(), mnGroup_);
            assert(mn_);
        }
        return mn_->addPath(tag, tiffPath, pRoot, std::move(object));
    } // TiffMnEntry::doAddPath