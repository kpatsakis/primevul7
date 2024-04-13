    template<typename tx, typename ty, typename tc>
    CImg<T>& draw_grid(const CImg<tx>& values_x, const CImg<ty>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U) {
      if (is_empty()) return *this;
      if (values_x) cimg_foroff(values_x,x) {
          const int xi = (int)values_x[x];
          if (xi>=0 && xi<width()) draw_line(xi,0,xi,_height - 1,color,opacity,pattern_x);
        }
      if (values_y) cimg_foroff(values_y,y) {
          const int yi = (int)values_y[y];
          if (yi>=0 && yi<height()) draw_line(0,yi,_width - 1,yi,color,opacity,pattern_y);
        }
      return *this;