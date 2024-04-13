    std::unique_ptr<Rename> Rename::clone() const
    {
        return std::unique_ptr<Rename>(clone_());
    }