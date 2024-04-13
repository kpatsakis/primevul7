      ~_functor2d_expr() { mp->end(); delete mp; }
      _functor2d_expr(const char *const expr):mp(0) {
        mp = new _cimg_math_parser(expr,0,CImg<T>::const_empty(),0);