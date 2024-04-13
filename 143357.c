    template<typename tc>
    CImg<T>& draw_triangle(int x0, int y0,
                           int x1, int y1,
                           int x2, int y2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           float bs0,
                           float bs1,
                           float bs2,
                           const float opacity=1) {
      if (is_empty()) return *this;
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (is_overlapped(texture))
        return draw_triangle(x0,y0,x1,y1,x2,y2,+texture,tx0,ty0,tx1,ty1,tx2,ty2,
                             bs0,bs1,bs2,opacity);

      if (y0>y1) cimg::swap(x0,x1,y0,y1,tx0,tx1,ty0,ty1,bs0,bs1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,tx0,tx2,ty0,ty2,bs0,bs2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,tx1,tx2,ty1,ty2,bs1,bs2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dx02 = x2 - x0, dx12 = x2 - x1,
        dy01 = std::max(1,y1 - y0), dy02 = std::max(1,y2 - y0), dy12 = std::max(1,y2 - y1),
        cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1),
        hdy01 = dy01*cimg::sign(dx01)/2, hdy02 = dy02*cimg::sign(dx02)/2, hdy12 = dy12*cimg::sign(dx12)/2,
        dtx01 = tx1 - tx0, dtx02 = tx2 - tx0, dtx12 = tx2 - tx1,
        dty01 = ty1 - ty0, dty02 = ty2 - ty0, dty12 = ty2 - ty1,
        hdy01tx = dy01*cimg::sign(dtx01)/2, hdy02tx = dy02*cimg::sign(dtx02)/2, hdy12tx = dy12*cimg::sign(dtx12)/2,
        hdy01ty = dy01*cimg::sign(dty01)/2, hdy02ty = dy02*cimg::sign(dty02)/2, hdy12ty = dy12*cimg::sign(dty12)/2;
      const float dbs01 = bs1 - bs0, dbs02 = bs2 - bs0, dbs12 = bs2 - bs1;

      const ulongT twhd = (ulongT)texture._width*texture._height*texture._depth;
      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const int yy0 = y - y0, yy1 = y - y1;
        int
          xm = y<y1?x0 + (dx01*yy0 + hdy01)/dy01:x1 + (dx12*yy1 + hdy12)/dy12,
          xM = x0 + (dx02*yy0 + hdy02)/dy02,
          txm = y<y1?tx0 + (dtx01*yy0 + hdy01tx)/dy01:tx1 + (dtx12*yy1 + hdy12tx)/dy12,
          txM = tx0 + (dtx02*yy0 + hdy02tx)/dy02,
          tym = y<y1?ty0 + (dty01*yy0 + hdy01ty)/dy01:ty1 + (dty12*yy1 + hdy12ty)/dy12,
          tyM = ty0 + (dty02*yy0 + hdy02ty)/dy02;
        float
          bsm = y<y1?(bs0 + dbs01*yy0/dy01):(bs1 + dbs12*yy1/dy12),
          bsM = bs0 + dbs02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,txm,txM,tym,tyM,bsm,bsM);
        if (xM>=0 || xm<=w1) {
          const int
            cxm = cimg::cut(xm,0,w1),
            cxM = cimg::cut(xM,0,w1);
          T *ptrd = data(cxm,y);
          const int
            dxmM = std::max(1,xM - xm), hdxmM = dxmM/2,
            dtxmM = txM - txm, dtymM = tyM - tym;
          const float dbsmM = bsM - bsm;

          for (int x = cxm; x<cxM; ++x) {
            const int
              xxm = x - xm,
              tx = (txm*dxmM + dtxmM*xxm + hdxmM)/dxmM,
              ty = (tym*dxmM + dtymM*xxm + hdxmM)/dxmM;
            const float cbs = cimg::cut(bsm + dbsmM*xxm/dxmM,0,2);
            const tc *const color = &texture._atXY(tx,ty);
            cimg_forC(*this,c) {
              const tc col = color[c*twhd];
              const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
              ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
            }
            ++ptrd;
          }
        }
      }
      return *this;