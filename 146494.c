    void TiffEncoder::setDirty(bool flag)
    {
        dirty_ = flag;
        setGo(geTraverse, !flag);
    }