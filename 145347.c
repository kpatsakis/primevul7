    Iptcdatum::Iptcdatum(const IptcKey& key,
                         const Value* pValue)
        : key_(key.clone())
    {
        if (pValue) value_ = pValue->clone();
    }