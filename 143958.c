    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity=1) {
      return draw_rectangle(x0,y0,0,x1,y1,_depth - 1,color,opacity);