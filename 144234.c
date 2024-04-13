      // Count parentheses/brackets level of each character of the expression.
      CImg<uintT> get_level(CImg<charT>& _expr) const {
        bool is_escaped = false, next_is_escaped = false;
        unsigned int mode = 0, next_mode = 0; // { 0=normal | 1=char-string | 2=vector-string
        CImg<uintT> res(_expr._width - 1);
        unsigned int *pd = res._data;
        int _level = 0;
        for (const char *ps = _expr._data; *ps && _level>=0; ++ps) {
          if (!is_escaped && !next_is_escaped && *ps=='\\') next_is_escaped = true;
          if (!is_escaped && *ps=='\'') { // Non-escaped character
            if (!mode && ps>_expr._data && *(ps - 1)=='[') next_mode = mode = 2; // Start vector-string
            else if (mode==2 && *(ps + 1)==']') next_mode = !mode; // End vector-string
            else if (mode<2) next_mode = mode?(mode = 0):1; // Start/end char-string
          }
          *(pd++) = (unsigned int)(mode>=1 || is_escaped?_level + (mode==1):
                                   *ps=='(' || *ps=='['?_level++:
                                   *ps==')' || *ps==']'?--_level:
                                   _level);
          mode = next_mode;
          is_escaped = next_is_escaped;
          next_is_escaped = false;
        }
        if (mode) {
          cimg::strellipsize(_expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Unterminated string literal, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      _expr._data);
        }
        if (_level) {
          cimg::strellipsize(_expr,64);
          throw CImgArgumentException("[" cimg_appname "_math_parser] "
                                      "CImg<%s>::%s: Unbalanced parentheses/brackets, in expression '%s'.",
                                      pixel_type(),_cimg_mp_calling_function,
                                      _expr._data);
        }
        return res;