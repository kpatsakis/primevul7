
    inline float option(const char *const name, const int argc, const char *const *const argv,
                        const float defaut, const char *const usage=0) {
      const char *const s = cimg::option(name,argc,argv,(char*)0);
      const float res = s?(float)cimg::atof(s):defaut;
      char *const tmp = new char[256];
      cimg_snprintf(tmp,256,"%g",res);
      cimg::option(name,0,0,tmp,usage);
      delete[] tmp;
      return res;