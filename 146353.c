    bool TiffMappingInfo::operator==(const TiffMappingInfo::Key& key) const
    {
        return    (   0 == strcmp("*", make_)
                   || 0 == strncmp(make_, key.m_.c_str(), strlen(make_)))
               && (Tag::all == extendedTag_ || key.e_ == extendedTag_)
               && key.g_ == group_;
    }