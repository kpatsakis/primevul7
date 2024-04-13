        bool operator==(const std::pair<const char*, const char*> &fromTo) const
            { return 0 == strcmp(from_, fromTo.first) && 0 == strcmp(to_, fromTo.second); }