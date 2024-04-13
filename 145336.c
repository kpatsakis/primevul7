    int XmpData::add(const XmpKey& key, const Value* value)
    {
        return add(Xmpdatum(key, value));
    }