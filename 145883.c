    void XmpParser::registerNs(const std::string& ns,
                               const std::string& prefix)
    {
        try {
            initialize();
            AutoLock autoLock(xmpLockFct_, pLockData_);
            SXMPMeta::DeleteNamespace(ns.c_str());
#ifdef EXV_ADOBE_XMPSDK
            SXMPMeta::RegisterNamespace(ns.c_str(), prefix.c_str(),nullptr);
#else
            SXMPMeta::RegisterNamespace(ns.c_str(), prefix.c_str());
#endif
        }
        catch (const XMP_Error& /* e */) {
            // throw Error(kerXMPToolkitError, e.GetID(), e.GetErrMsg());
        }
    } // XmpParser::registerNs