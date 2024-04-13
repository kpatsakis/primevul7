    void TiffReader::postProcess()
    {
        setMnState(); // All components to be post-processed must be from the Makernote
        postProc_ = true;
        for (PostList::const_iterator pos = postList_.begin(); pos != postList_.end(); ++pos) {
            (*pos)->accept(*this);
        }
        postProc_ = false;
        setOrigState();
    }