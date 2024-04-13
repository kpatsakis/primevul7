      // { 0=constant | 1=integer constant | 2=positive integer constant | 3=strictly-positive integer constant }
      void check_constant(const unsigned int arg, const unsigned int n_arg,
                          const unsigned int mode,
                          char *const ss, char *const se, const char saved_char) {
        _cimg_mp_check_type(arg,n_arg,1,0);
        if (!(_cimg_mp_is_constant(arg) &&
              (!mode || (double)(int)mem[arg]==mem[arg]) &&
              (mode<2 || mem[arg]>=(mode==3)))) {
          const char *s_arg = !n_arg?"":n_arg==1?"First ":n_arg==2?"Second ":n_arg==3?"Third ":
            n_arg==4?"Fourth ":n_arg==5?"Fifth ":n_arg==6?"Sixth ":n_arg==7?"Seventh ":n_arg==8?"Eighth ":
            n_arg==9?"Ninth ":"One of the ";
          *se = saved_char;
          char *const s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s (of type '%s') is not a%s constant, "
                                      "in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_arg?"argument":"Argument",s_type(arg)._data,
                                      !mode?"":mode==1?"n integer":
                                      mode==2?" positive integer":" strictly positive integer",
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        }