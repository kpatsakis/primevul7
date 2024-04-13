    Insert* Insert::clone_() const
    {
        return new Insert(*this);
    }