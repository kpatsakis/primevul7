    std::unique_ptr<Print> Print::clone() const
    {
        return std::unique_ptr<Print>(clone_());
    }