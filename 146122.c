    TiffComponent* TiffSubIfd::doAddPath(uint16_t tag,
                                         TiffPath& tiffPath,
                                         TiffComponent* const pRoot,
                                         TiffComponent::UniquePtr object)
    {
        assert(!tiffPath.empty());
        const TiffPathItem tpi1 = tiffPath.top();
        tiffPath.pop();
        if (tiffPath.empty()) {
            // If the last element in the path is the sub-IFD tag itself we're done.
            // But that shouldn't happen - see TiffDirectory::doAddPath
            return this;
        }
        const TiffPathItem tpi2 = tiffPath.top();
        tiffPath.push(tpi1);
        TiffComponent* tc = 0;
        for (Ifds::iterator i = ifds_.begin(); i != ifds_.end(); ++i) {
            if ((*i)->group() == tpi2.group()) {
                tc = *i;
                break;
            }
        }
        if (tc == 0) {
            if (tiffPath.size() == 1 && object.get() != 0) {
                tc = addChild(std::move(object));
            }
            else {
                TiffComponent::UniquePtr atc(new TiffDirectory(tpi1.tag(), tpi2.group()));
                tc = addChild(std::move(atc));
            }
            setCount(static_cast<uint32_t>(ifds_.size()));
        }
        return tc->addPath(tag, tiffPath, pRoot, std::move(object));
    } // TiffSubIfd::doAddPath