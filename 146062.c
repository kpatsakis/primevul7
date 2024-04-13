    static std::string temporaryPath()
    {
        static int count = 0;

#if defined(_MSC_VER) || defined(__MINGW__)
        EnterCriticalSection(&cs);
        char lpTempPathBuffer[MAX_PATH];
        GetTempPath(MAX_PATH, lpTempPathBuffer);
        std::string tmp(lpTempPathBuffer);
        tmp += "\\";
        HANDLE process = 0;
        DWORD pid = ::GetProcessId(process);
#else
        pid_t pid = ::getpid();
        pthread_mutex_lock(&cs);
        std::string tmp = "/tmp/";
#endif
        char sCount[12];
        sprintf(sCount, "_%d", ++count);

        std::string result = tmp + Exiv2::toString(pid) + sCount;
        if (Exiv2::fileExists(result))
            std::remove(result.c_str());

#if defined(_MSC_VER) || defined(__MINGW__)
        LeaveCriticalSection(&cs);
#else
        pthread_mutex_unlock(&cs);
#endif

        return result;
    }