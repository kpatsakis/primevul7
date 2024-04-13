    std::ostream& XmpArrayValue::write(std::ostream& os) const
    {
        for (std::vector<std::string>::const_iterator i = value_.begin();
             i != value_.end(); ++i) {
            if (i != value_.begin()) os << ", ";
            os << *i;
        }
        return os;
    }