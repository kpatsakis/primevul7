    TiffDirectory::~TiffDirectory()
    {
        for (Components::iterator i = components_.begin(); i != components_.end(); ++i) {
            delete *i;
        }
        delete pNext_;
    }