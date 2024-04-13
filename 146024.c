    std::unique_ptr<FixIso> FixIso::clone() const
    {
        return std::unique_ptr<FixIso>(clone_());
    }