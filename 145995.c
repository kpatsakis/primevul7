    std::unique_ptr<Task> TaskFactory::create(TaskType type)
    {
        auto i = registry_.find(type);
        if (i != registry_.end() && i->second != 0) {
            Task* t = i->second;
            return t->clone();
        }
        return nullptr;
    }