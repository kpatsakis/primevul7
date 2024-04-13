    std::unique_ptr<Task> Task::clone() const
    {
        return std::unique_ptr<Task>(clone_());
    }