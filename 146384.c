    int DataValue::read(const std::string& buf)
    {
        std::istringstream is(buf);
        int tmp;
        ValueType val;
        while (!(is.eof())) {
            is >> tmp;
            if (is.fail()) return 1;
            val.push_back(static_cast<byte>(tmp));
        }
        value_.swap(val);
        return 0;
    }