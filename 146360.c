    std::unique_ptr<FixCom> FixCom::clone() const
    {
        return std::unique_ptr<FixCom>(clone_());
    }