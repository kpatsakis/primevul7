        bool operator==(const std::string& name) const
        {
            return 0 == strcmp(name_, name.c_str());
        }