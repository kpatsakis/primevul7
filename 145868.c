    TiffComponent* TiffDirectory::doAddPath(uint16_t tag,
                                            TiffPath& tiffPath,
                                            TiffComponent* const pRoot,
                                            TiffComponent::UniquePtr object)
    {
        assert(tiffPath.size() > 1);
        tiffPath.pop();
        const TiffPathItem tpi = tiffPath.top();

        TiffComponent* tc = 0;
        // Try to use an existing component if there is still at least one
        // composite tag on the stack or the tag to add is the MakerNote tag.
        // This is used to prevent duplicate entries. Sub-IFDs also, but the > 1
        // condition takes care of them, see below.
        if (   tiffPath.size() > 1
            || (tpi.extendedTag() == 0x927c && tpi.group() == exifId)) {
            if (tpi.extendedTag() == Tag::next) {
                tc = pNext_;
            }
            else {
                for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
                    if ((*i)->tag() == tpi.tag() && (*i)->group() == tpi.group()) {
                        tc = *i;
                        break;
                    }
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

            // Prevent dangling sub-IFD tags: Do not add a sub-IFD component without children.
            // Todo: How to check before creating the component?
            if (tiffPath.size() == 1 && dynamic_cast<TiffSubIfd*>(atc.get()) != 0) return 0;

            if (tpi.extendedTag() == Tag::next) {
                tc = this->addNext(std::move(atc));
            }
            else {
                tc = this->addChild(std::move(atc));
            }
        }
        return tc->addPath(tag, tiffPath, pRoot, std::move(object));
    } // TiffDirectory::doAddPath