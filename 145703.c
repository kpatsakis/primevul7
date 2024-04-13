    bool cmpGroupLt(TiffComponent const* lhs, TiffComponent const* rhs)
    {
        assert(lhs != 0);
        assert(rhs != 0);
        return lhs->group() < rhs->group();
    }