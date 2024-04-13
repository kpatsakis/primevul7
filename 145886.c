    TaskFactory::~TaskFactory()
    {
        for (auto it = registry_.begin(); it != registry_.end(); ++it) {
            delete it->second;
        }
    }