    std::unique_ptr<Modify> Modify::clone() const
    {
        return std::unique_ptr<Modify>(clone_());
    }