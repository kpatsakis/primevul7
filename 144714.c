    template<typename tc>
    CImg<T>& draw_ellipse(const int x0, const int y0, const float r1, const float r2, const float angle,
                          const tc *const color, const float opacity, const unsigned int pattern) {
      if (pattern) _draw_ellipse(x0,y0,r1,r2,angle,color,opacity,pattern,false);
      return *this;