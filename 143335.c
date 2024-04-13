    template<typename tc>
    CImg<T>& _draw_triangle(int x0, int y0,
                            int x1, int y1,
                            int x2, int y2,
                            const tc *const color, const float opacity,
                            const float brightness) {
      if (y0>y1) cimg::swap(x0,x1,y0,y1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int
        h1 = height() - 1,
        dx01 = x1 - x0, dx02 = x2 - x0, dx12 = x2 - x1,
        dy01 = std::max(1,y1 - y0), dy02 = std::max(1,y2 - y0), dy12 = std::max(1,y2 - y1),
        cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1),
        hdy01 = dy01*cimg::sign(dx01)/2, hdy02 = dy02*cimg::sign(dx02)/2, hdy12 = dy12*cimg::sign(dx12)/2;

      const float cbs = cimg::cut(brightness,0,2);
      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const int yy0 = y - y0, yy1 = y - y1;
        int
          xm = y<y1?x0 + (dx01*yy0 + hdy01)/dy01:x1 + (dx12*yy1 + hdy12)/dy12,
          xM = x0 + (dx02*yy0 + hdy02)/dy02;
        if (xm>xM) cimg::swap(xm,xM);
        cimg_draw_scanline(xm,xM,y,color,opacity,cbs);
      }
      return *this;