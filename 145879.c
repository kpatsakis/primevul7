    std::unique_ptr<Extract> Extract::clone() const
    {
        return std::unique_ptr<Extract>(clone_());
    }