    // Return string that identifies the running OS.
    inline const char *stros() {
#if defined(linux) || defined(__linux) || defined(__linux__)
      static const char *const str = "Linux";
#elif defined(sun) || defined(__sun)
      static const char *const str = "Sun OS";
#elif defined(BSD) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__FreeBSD__) || defined (__DragonFly__)
      static const char *const str = "BSD";
#elif defined(sgi) || defined(__sgi)
      static const char *const str = "Irix";
#elif defined(__MACOSX__) || defined(__APPLE__)
      static const char *const str = "Mac OS";
#elif defined(unix) || defined(__unix) || defined(__unix__)
      static const char *const str = "Generic Unix";
#elif defined(_MSC_VER) || defined(WIN32)  || defined(_WIN32) || defined(__WIN32__) || \
  defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
      static const char *const str = "Windows";
#else
      const char
        *const _str1 = std::getenv("OSTYPE"),
        *const _str2 = _str1?_str1:std::getenv("OS"),
        *const str = _str2?_str2:"Unknown OS";
#endif
      return str;