    template<typename tc>
    CImg<T>& draw_spline(const int x0, const int y0, const float u0, const float v0,
                         const int x1, const int y1, const float u1, const float v1,
                         const tc *const color, const float opacity=1,
                         const float precision=0.25, const unsigned int pattern=~0U,
                         const bool init_hatch=true) {
      if (is_empty()) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_spline(): Specified color is (null).",
                                    cimg_instance);
      if (x0==x1 && y0==y1) return draw_point(x0,y0,color,opacity);
      bool ninit_hatch = init_hatch;
      const float
        ax = u0 + u1 + 2*(x0 - x1),
        bx = 3*(x1 - x0) - 2*u0 - u1,
        ay = v0 + v1 + 2*(y0 - y1),
        by = 3*(y1 - y0) - 2*v0 - v1,
        _precision = 1/(cimg::hypot((float)x0 - x1,(float)y0 - y1)*(precision>0?precision:1));
      int ox = x0, oy = y0;
      for (float t = 0; t<1; t+=_precision) {
        const float t2 = t*t, t3 = t2*t;
        const int
          nx = (int)(ax*t3 + bx*t2 + u0*t + x0),
          ny = (int)(ay*t3 + by*t2 + v0*t + y0);
        draw_line(ox,oy,nx,ny,color,opacity,pattern,ninit_hatch);
        ninit_hatch = false;
        ox = nx; oy = ny;
      }
      return draw_line(ox,oy,x1,y1,color,opacity,pattern,false);