    TiffBinaryArray::~TiffBinaryArray()
    {
        for (Components::iterator i = elements_.begin(); i != elements_.end(); ++i) {
            delete *i;
        }
    }