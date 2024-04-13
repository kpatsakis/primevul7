    void TiffEncoder::visitDirectoryNext(TiffDirectory* object)
    {
        // Update type and count in IFD entries, in case they changed
        assert(object != 0);

        byte* p = object->start() + 2;
        for (TiffDirectory::Components::iterator i = object->components_.begin();
             i != object->components_.end(); ++i) {
            p += updateDirEntry(p, byteOrder(), *i);
        }
    }