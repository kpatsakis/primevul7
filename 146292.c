    long XmpValue::size() const
    {
        std::ostringstream os;
        write(os);
        return static_cast<long>(os.str().size());
    }