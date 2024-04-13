    void TiffFinder::init(uint16_t tag, IfdId group)
    {
        tag_ = tag;
        group_ = group;
        tiffComponent_ = 0;
        setGo(geTraverse, true);
    }