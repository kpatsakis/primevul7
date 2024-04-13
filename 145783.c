    void XmpParser::terminate()
    {
        XmpProperties::unregisterNs();
        if (initialized_) {
#ifdef EXV_HAVE_XMP_TOOLKIT
            SXMPMeta::Terminate();
#endif
            initialized_ = false;
        }
    }