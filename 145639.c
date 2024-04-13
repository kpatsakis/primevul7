    bool cmpTagLt(TiffComponent const* lhs, TiffComponent const* rhs)
    {
        assert(lhs != 0);
        assert(rhs != 0);
        if (lhs->tag() != rhs->tag()) return lhs->tag() < rhs->tag();
        return lhs->idx() < rhs->idx();
    }