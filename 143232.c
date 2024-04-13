    template<typename tc>
    CImg<T>& draw_line(int x0, int y0,
                       int x1, int y1,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const bool init_hatch=true) {
      if (is_empty() || !opacity || !pattern ||
          std::min(y0,y1)>=height() || std::max(y0,y1)<0 ||
          std::min(x0,x1)>=width() || std::max(x0,x1)<0) return *this;

      int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dy01 = y1 - y0;

      const bool is_horizontal = cimg::abs(dx01)>cimg::abs(dy01);
      if (is_horizontal) cimg::swap(x0,y0,x1,y1,w1,h1,dx01,dy01);
      if (pattern==~0U && y0>y1) {
        cimg::swap(x0,x1,y0,y1);
        dx01*=-1; dy01*=-1;
      }

      static unsigned int hatch = ~0U - (~0U>>1);
      if (init_hatch) hatch = ~0U - (~0U>>1);
      cimg_init_scanline(opacity);
      const int
        step = y0<=y1?1:-1,hdy01 = dy01*cimg::sign(dx01)/2,
        cy0 = cimg::cut(y0,0,h1), cy1 = cimg::cut(y1,0,h1) + step;
      dy01+=dy01?0:1;

      for (int y = cy0; y!=cy1; y+=step) {
        const int
          yy0 = y - y0,
          x = x0 + (dx01*yy0 + hdy01)/dy01;
        if (x>=0 && x<=w1 && pattern&hatch) {
          T *const ptrd = is_horizontal?data(y,x):data(x,y);
          cimg_forC(*this,c) {
            const T val = color[c];
            ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
          }
        }
        if (!(hatch>>=1)) hatch = ~0U - (~0U>>1);
      }
      return *this;