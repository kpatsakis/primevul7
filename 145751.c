    static std::string toLowerCase(std::string a)
    {
        for(size_t i=0 ; i < a.length() ; i++)
        {
            a[i]=tolower(a[i]);
        }
        return a;
    }