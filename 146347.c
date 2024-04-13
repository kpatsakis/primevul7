    bool TiffTreeStruct::operator==(const TiffTreeStruct::Key& key) const
    {
        return key.r_ == root_ && key.g_ == group_;
    }