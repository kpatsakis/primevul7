    template<typename tc>
    CImg<T>& draw_fill(const int x0, const int y0,
                       const tc *const color, const float opacity=1,
                       const float tolerance=0, const bool is_high_connexity=false) {
      CImg<ucharT> tmp;
      return draw_fill(x0,y0,0,color,opacity,tmp,tolerance,is_high_connexity);