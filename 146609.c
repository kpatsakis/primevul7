    Iptcdatum& Iptcdatum::operator=(const uint16_t& value)
    {
        UShortValue::UniquePtr v(new UShortValue);
        v->value_.push_back(value);
        value_ = std::move(v);
        return *this;
    }