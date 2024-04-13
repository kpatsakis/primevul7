      // If 'N' is not zero, it also restricts the vectors to be of size N only.
      void check_type(const unsigned int arg, const unsigned int n_arg,
                      const unsigned int mode, const unsigned int N,
                      char *const ss, char *const se, const char saved_char) {
        const bool
          is_scalar = _cimg_mp_is_scalar(arg),
          is_vector = _cimg_mp_is_vector(arg) && (!N || _cimg_mp_size(arg)==N);
        bool cond = false;
        if (mode&1) cond|=is_scalar;
        if (mode&2) cond|=is_vector;
        if (!cond) {
          const char *s_arg;
          if (*s_op!='F') s_arg = !n_arg?"":n_arg==1?"Left-hand ":"Right-hand ";
          else s_arg = !n_arg?"":n_arg==1?"First ":n_arg==2?"Second ":n_arg==3?"Third ":
                 n_arg==4?"Fourth ":n_arg==5?"Fifth ":n_arg==6?"Sixth ":n_arg==7?"Seventh ":n_arg==8?"Eighth":
                 n_arg==9?"Ninth":"One of the ";
          CImg<charT> sb_type(32);
          if (mode==1) cimg_snprintf(sb_type,sb_type._width,"'scalar'");
          else if (mode==2) {
            if (N) cimg_snprintf(sb_type,sb_type._width,"'vector%u'",N);
            else cimg_snprintf(sb_type,sb_type._width,"'vector'");
          } else {
            if (N) cimg_snprintf(sb_type,sb_type._width,"'scalar' or 'vector%u'",N);
            else cimg_snprintf(sb_type,sb_type._width,"'scalar' or 'vector'");
          }
          *se = saved_char;
          char *const s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s %s%s has invalid type '%s' (should be %s), "
                                      "in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s_arg,*s_op=='F'?(*s_arg?"argument":"Argument"):(*s_arg?"operand":"Operand"),
                                      s_type(arg)._data,sb_type._data,
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        }