
    inline const char* option(const char *const name, const int argc, const char *const *const argv,
                              const char *const defaut, const char *const usage=0) {
      return option(name,argc,argv,defaut,usage,false);