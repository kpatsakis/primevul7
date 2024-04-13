      ~_functor4d_streamline_expr() { mp->end(); delete mp; }
      _functor4d_streamline_expr(const char *const expr):mp(0) {
        mp = new _cimg_math_parser(expr,"streamline",CImg<T>::const_empty(),0);