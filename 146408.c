        ~BlockMap()
        {
            if (data_) {
                std::free(data_);
                data_ = nullptr;
            }
        }