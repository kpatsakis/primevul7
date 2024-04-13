    template<typename tc>
    CImg<T>& draw_triangle(const int x0, const int y0,
                           const int x1, const int y1,
                           const int x2, const int y2,
                           const tc *const color, const float opacity,
                           const unsigned int pattern) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      draw_line(x0,y0,x1,y1,color,opacity,pattern,true).
        draw_line(x1,y1,x2,y2,color,opacity,pattern,false).
        draw_line(x2,y2,x0,y0,color,opacity,pattern,false);
      return *this;