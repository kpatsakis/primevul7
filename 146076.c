    bool Print::grepTag(const std::string& key)
    {
        bool result = Params::instance().greps_.empty();
        for (auto g = Params::instance().greps_.begin(); !result && g != Params::instance().greps_.end(); ++g)
        {
            std::smatch m;
            result = std::regex_search(key, m, *g);
        }
        return result;
    }