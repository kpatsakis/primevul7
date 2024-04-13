    std::unique_ptr<Insert> Insert::clone() const
    {
        return std::unique_ptr<Insert>(clone_());
    }