    TaskFactory& TaskFactory::instance()
    {
        static TaskFactory ins;
        return ins;
    }