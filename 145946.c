    BasicIo::UniquePtr ImageFactory::createIo(const std::string& path, bool useCurl)
    {
        Protocol fProt = fileProtocol(path);

#ifdef EXV_USE_SSH
        if (fProt == pSsh || fProt == pSftp) {
            return BasicIo::UniquePtr(new SshIo(path)); // may throw
        }
#endif

#ifdef EXV_USE_CURL
        if (useCurl && (fProt == pHttp || fProt == pHttps || fProt == pFtp)) {
            return BasicIo::UniquePtr(new CurlIo(path)); // may throw
        }
#endif

        if (fProt == pHttp)
            return BasicIo::UniquePtr(new HttpIo(path)); // may throw
        if (fProt == pFileUri)
            return BasicIo::UniquePtr(new FileIo(pathOfFileUrl(path)));
        if (fProt == pStdin || fProt == pDataUri)
            return BasicIo::UniquePtr(new XPathIo(path)); // may throw

        return BasicIo::UniquePtr(new FileIo(path));

        (void)(useCurl);
    }