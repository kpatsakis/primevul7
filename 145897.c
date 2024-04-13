    std::unique_ptr<Adjust> Adjust::clone() const
    {
        return std::unique_ptr<Adjust>(clone_());
    }