    BasicIo::UniquePtr ImageFactory::createIo(const std::wstring& wpath, bool useCurl)
    {
        Protocol fProt = fileProtocol(wpath);
#if EXV_USE_SSH == 1
        if (fProt == pSsh || fProt == pSftp) {
            return BasicIo::UniquePtr(new SshIo(wpath));
        }
#endif
#if EXV_USE_CURL == 1
        if (useCurl && (fProt == pHttp || fProt == pHttps || fProt == pFtp)) {
            return BasicIo::UniquePtr(new CurlIo(wpath));
        }
#endif
        if (fProt == pHttp)
            return BasicIo::UniquePtr(new HttpIo(wpath));
        if (fProt == pFileUri)
            return BasicIo::UniquePtr(new FileIo(pathOfFileUrl(wpath)));
        if (fProt == pStdin || fProt == pDataUri)
            return BasicIo::UniquePtr(new XPathIo(wpath)); // may throw
        return BasicIo::UniquePtr(new FileIo(wpath));
    }