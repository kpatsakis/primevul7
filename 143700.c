    template<typename tc>
    CImg<T>& draw_point(const int x0, const int y0,
                        const tc *const color, const float opacity=1) {
      return draw_point(x0,y0,0,color,opacity);