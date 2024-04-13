    void TiffCreator::getPath(TiffPath& tiffPath,
                              uint32_t  extendedTag,
                              IfdId     group,
                              uint32_t  root)
    {
        const TiffTreeStruct* ts = 0;
        do {
            tiffPath.push(TiffPathItem(extendedTag, group));
            ts = find(tiffTreeStruct_, TiffTreeStruct::Key(root, group));
            assert(ts != 0);
            extendedTag = ts->parentExtTag_;
            group = ts->parentGroup_;
        } while (!(ts->root_ == root && ts->group_ == ifdIdNotSet));

    } // TiffCreator::getPath