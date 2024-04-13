    void XmpParser::unregisterNs(const std::string& /*ns*/)
    {
#ifdef EXV_HAVE_XMP_TOOLKIT
        try {
// Throws XMP Toolkit error 8: Unimplemented method XMPMeta::DeleteNamespace
//          SXMPMeta::DeleteNamespace(ns.c_str());
        }
        catch (const XMP_Error& e) {
            throw Error(kerXMPToolkitError, e.GetID(), e.GetErrMsg());
        }
#endif
    } // XmpParser::unregisterNs