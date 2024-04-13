    std::string Value::toString() const
    {
        std::ostringstream os;
        write(os);
        ok_ = !os.fail();
        return os.str();
    }