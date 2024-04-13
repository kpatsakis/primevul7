    TiffComponent* TiffBinaryArray::doAddPath(uint16_t tag,
                                              TiffPath& tiffPath,
                                              TiffComponent* const pRoot,
                                              TiffComponent::UniquePtr object)
    {
        pRoot_ = pRoot;
        if (tiffPath.size() == 1) {
            // An unknown complex binary array has no children and acts like a standard TIFF entry
            return this;
        }
        tiffPath.pop();
        const TiffPathItem tpi = tiffPath.top();
        // Initialize the binary array (if it is a complex array)
        initialize(tpi.group());
        TiffComponent* tc = 0;
        // Todo: Duplicates are not allowed!
        // To allow duplicate entries, we only check if the new component already
        // exists if there is still at least one composite tag on the stack
        if (tiffPath.size() > 1) {
            for (Components::iterator i = elements_.begin(); i != elements_.end(); ++i) {
                if ((*i)->tag() == tpi.tag() && (*i)->group() == tpi.group()) {
                    tc = *i;
                    break;
                }
            }
        }
        if (tc == 0) {
            TiffComponent::UniquePtr atc;
            if (tiffPath.size() == 1 && object.get() != 0) {
                atc = std::move(object);
            }
            else {
                atc = TiffCreator::create(tpi.extendedTag(), tpi.group());
            }
            assert(atc.get() != 0);
            assert(tpi.extendedTag() != Tag::next);
            tc = addChild(std::move(atc));
            setCount(static_cast<uint32_t>(elements_.size()));
        }
        return tc->addPath(tag, tiffPath, pRoot, std::move(object));
    } // TiffBinaryArray::doAddPath