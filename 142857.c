    //! Return options specified on the command line.
    inline const char* option(const char *const name, const int argc, const char *const *const argv,
                              const char *const defaut, const char *const usage, const bool reset_static) {
      static bool first = true, visu = false;
      if (reset_static) { first = true; return 0; }
      const char *res = 0;
      if (first) {
        first = false;
        visu = cimg::option("-h",argc,argv,(char*)0,(char*)0,false)!=0;
        visu |= cimg::option("-help",argc,argv,(char*)0,(char*)0,false)!=0;
        visu |= cimg::option("--help",argc,argv,(char*)0,(char*)0,false)!=0;
      }
      if (!name && visu) {
        if (usage) {
          std::fprintf(cimg::output(),"\n %s%s%s",cimg::t_red,cimg::basename(argv[0]),cimg::t_normal);
          std::fprintf(cimg::output(),": %s",usage);
          std::fprintf(cimg::output()," (%s, %s)\n\n",cimg_date,cimg_time);
        }
        if (defaut) std::fprintf(cimg::output(),"%s\n",defaut);
      }
      if (name) {
        if (argc>0) {
          int k = 0;
          while (k<argc && std::strcmp(argv[k],name)) ++k;
          res = (k++==argc?defaut:(k==argc?argv[--k]:argv[k]));
        } else res = defaut;
        if (visu && usage) std::fprintf(cimg::output(),"    %s%-16s%s %-24s %s%s%s\n",
                                        cimg::t_bold,name,cimg::t_normal,res?res:"0",
                                        cimg::t_green,usage,cimg::t_normal);
      }
      return res;