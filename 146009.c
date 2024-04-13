    std::string DataValue::toString(long n) const
    {
        std::ostringstream os;
        os << static_cast<int>(value_[n]);
        ok_ = !os.fail();
        return os.str();
    }