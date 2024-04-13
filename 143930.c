
    inline char option(const char *const name, const int argc, const char *const *const argv,
                       const char defaut, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const char res = s?*s:defaut;
      char tmp[8];
      *tmp = res; tmp[1] = 0;
      cimg::option(name,0,0,tmp,usage);
      return res;