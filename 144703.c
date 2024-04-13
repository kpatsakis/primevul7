
    inline int option(const char *const name, const int argc, const char *const *const argv,
                      const int defaut, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const int res = s?std::atoi(s):defaut;
      char *const tmp = new char[256];
      cimg_snprintf(tmp,256,"%d",res);
      cimg::option(name,0,0,tmp,usage);
      delete[] tmp;
      return res;