    template<typename tf>
    static CImg<floatT> elevation3d(CImgList<tf>& primitives, const char *const expression,
                                    const float x0, const float y0, const float x1, const float y1,
                                    const int size_x=256, const int size_y=256) {
      const _functor2d_expr func(expression);
      return elevation3d(primitives,func,x0,y0,x1,y1,size_x,size_y);