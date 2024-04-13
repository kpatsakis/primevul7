    ~Data()
    {
        if (deleteStream) delete is;

        for (size_t i = 0; i < parts.size(); i++)
            delete parts[i];
    }