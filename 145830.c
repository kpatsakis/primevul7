    std::ostream& AsciiValue::write(std::ostream& os) const
    {
        // Write only up to the first '\0' (if any)
        std::string::size_type pos = value_.find_first_of('\0');
        if (pos == std::string::npos) pos = value_.size();
        return os << value_.substr(0, pos);
    }