    bool Print::keyTag(const std::string& key)
    {
        bool result = Params::instance().keys_.empty();
        for (auto k = Params::instance().keys_.begin(); !result && k != Params::instance().keys_.end(); ++k) {
            result = key.compare(*k) == 0;
        }
        return result;
    }