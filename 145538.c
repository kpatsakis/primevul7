        bool operator==(const Key& key) const
        {
            return    key.tag_ == tag_
                   && 0 == strncmp(key.ver_, ver_, strlen(ver_))
                   && (size_ == 0 || key.size_ == size_);
        }