    void TiffCopier::copyObject(TiffComponent* object)
    {
        assert(object != 0);

        if (pHeader_->isImageTag(object->tag(), object->group(), pPrimaryGroups_)) {
            TiffComponent::UniquePtr clone = object->clone();
            // Assumption is that the corresponding TIFF entry doesn't exist
            TiffPath tiffPath;
            TiffCreator::getPath(tiffPath, object->tag(), object->group(), root_);
            pRoot_->addPath(object->tag(), tiffPath, pRoot_, std::move(clone));
#ifdef DEBUG
            ExifKey key(object->tag(), groupName(object->group()));
            std::cerr << "Copied " << key << "\n";
#endif
        }
    }