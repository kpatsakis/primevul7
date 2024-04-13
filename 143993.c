
    inline bool option(const char *const name, const int argc, const char *const *const argv,
                       const bool defaut, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const bool res = s?(cimg::strcasecmp(s,"false") && cimg::strcasecmp(s,"off") && cimg::strcasecmp(s,"0")):defaut;
      cimg::option(name,0,0,res?"true":"false",usage);
      return res;