    static bool matchi(const std::string key,const char* substr)
    {
        return toLowerCase(key).find(substr) != std::string::npos;
    }