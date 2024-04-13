    template<typename tz, typename tc>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const tc *const color, const float opacity=1,
                           const float brightness=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Specified color is (null).",
                                    cimg_instance);
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dx02 = x2 - x0, dx12 = x2 - x1,
        dy01 = std::max(1,y1 - y0), dy02 = std::max(1,y2 - y0), dy12 = std::max(1,y2 - y1),
        cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1),
        hdy01 = dy01*cimg::sign(dx01)/2, hdy02 = dy02*cimg::sign(dx02)/2, hdy12 = dy12*cimg::sign(dx12)/2;
      const float diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1;

      const float cbs = cimg::cut(brightness,0,2);
      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const int yy0 = y - y0, yy1 = y - y1;
        int
          xm = y<y1?x0 + (dx01*yy0 + hdy01)/dy01:x1 + (dx12*yy1 + hdy12)/dy12,
          xM = x0 + (dx02*yy0 + hdy02)/dy02;
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM);
        if (xM>=0 || xm<=w1) {
          const int
            cxm = cimg::cut(xm,0,w1),
            cxM = cimg::cut(xM,0,w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const int dxmM = std::max(1,xM - xm);
          const float dizmM = izM - izm;

          for (int x = cxm; x<cxM; ++x) {
            const int xxm = x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              cimg_forC(*this,c) {
                const Tfloat val = cbs<=1?color[c]*cbs:(2 - cbs)*color[c] + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;