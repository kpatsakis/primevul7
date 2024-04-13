    void XmpParser::registeredNamespaces(Exiv2::Dictionary& dict)
    {
        bool bInit = !initialized_;
        try {
            if (bInit) initialize();
            SXMPMeta::DumpNamespaces(nsDumper,&dict);
            if (bInit) terminate();
        } catch (const XMP_Error& e) {
            throw Error(kerXMPToolkitError, e.GetID(), e.GetErrMsg());
        }
    }