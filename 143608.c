    template<typename tc>
    CImg<T>& draw_rectangle(const int x0, const int y0,
                            const int x1, const int y1,
                            const tc *const color, const float opacity,
                            const unsigned int pattern) {
      if (is_empty()) return *this;
      if (y0==y1) return draw_line(x0,y0,x1,y0,color,opacity,pattern,true);
      if (x0==x1) return draw_line(x0,y0,x0,y1,color,opacity,pattern,true);
      const int
        nx0 = x0<x1?x0:x1, nx1 = x0^x1^nx0,
        ny0 = y0<y1?y0:y1, ny1 = y0^y1^ny0;
      if (ny1==ny0 + 1) return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
                      draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false);
      return draw_line(nx0,ny0,nx1,ny0,color,opacity,pattern,true).
        draw_line(nx1,ny0 + 1,nx1,ny1 - 1,color,opacity,pattern,false).
        draw_line(nx1,ny1,nx0,ny1,color,opacity,pattern,false).
        draw_line(nx0,ny1 - 1,nx0,ny0 + 1,color,opacity,pattern,false);