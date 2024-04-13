      // Check that listin or listout are not empty.
      void check_list(const bool is_out,
                      char *const ss, char *const se, const char saved_char) {
        if ((!is_out && !listin) || (is_out && !listout)) {
          *se = saved_char;
          char *const s0 = ss - 4>expr._data?ss - 4:expr._data;
          cimg::strellipsize(s0,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: %s%s Invalid call with an empty image list, "
                                      "in expression '%s%s%s'.",
                                      pixel_type(),_cimg_mp_calling_function,s_op,*s_op?":":"",
                                      s0!=expr._data?"...":"",s0,se<&expr.back()?"...":"");
        }