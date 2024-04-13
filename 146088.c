    std::unique_ptr<Erase> Erase::clone() const
    {
        return std::unique_ptr<Erase>(clone_());
    }