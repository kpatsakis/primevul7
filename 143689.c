    template<typename tz, typename tc, typename tl>
    CImg<T>& draw_triangle(CImg<tz>& zbuffer,
                           int x0, int y0, const float z0,
                           int x1, int y1, const float z1,
                           int x2, int y2, const float z2,
                           const CImg<tc>& texture,
                           int tx0, int ty0,
                           int tx1, int ty1,
                           int tx2, int ty2,
                           const CImg<tl>& light,
                           int lx0, int ly0,
                           int lx1, int ly1,
                           int lx2, int ly2,
                           const float opacity=1) {
      if (is_empty() || z0<=0 || z1<=0 || z2<=0) return *this;
      if (!is_sameXY(zbuffer))
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Instance and specified Z-buffer (%u,%u,%u,%u,%p) have "
                                    "different dimensions.",
                                    cimg_instance,
                                    zbuffer._width,zbuffer._height,zbuffer._depth,zbuffer._spectrum,zbuffer._data);
      if (texture._depth>1 || texture._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    texture._width,texture._height,texture._depth,texture._spectrum,texture._data);
      if (light._depth>1 || light._spectrum<_spectrum)
        throw CImgArgumentException(_cimg_instance
                                    "draw_triangle(): Invalid specified light texture (%u,%u,%u,%u,%p).",
                                    cimg_instance,light._width,light._height,light._depth,light._spectrum,light._data);
      if (is_overlapped(texture))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             +texture,tx0,ty0,tx1,ty1,tx2,ty2,light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);
      if (is_overlapped(light))
        return draw_triangle(zbuffer,x0,y0,z0,x1,y1,z1,x2,y2,z2,
                             texture,tx0,ty0,tx1,ty1,tx2,ty2,+light,lx0,ly0,lx1,ly1,lx2,ly2,opacity);

      float iz0 = 1/z0, iz1 = 1/z1, iz2 = 1/z2;
      if (y0>y1) cimg::swap(x0,x1,y0,y1,iz0,iz1,tx0,tx1,ty0,ty1,lx0,lx1,ly0,ly1);
      if (y0>y2) cimg::swap(x0,x2,y0,y2,iz0,iz2,tx0,tx2,ty0,ty2,lx0,lx2,ly0,ly2);
      if (y1>y2) cimg::swap(x1,x2,y1,y2,iz1,iz2,tx1,tx2,ty1,ty2,lx1,lx2,ly1,ly2);
      if (y2<0 || y0>=height() || cimg::min(x0,x1,x2)>=width() || cimg::max(x0,x1,x2)<0 || !opacity) return *this;

      const int
        w1 = width() - 1, h1 = height() - 1,
        dx01 = x1 - x0, dx02 = x2 - x0, dx12 = x2 - x1,
        dy01 = std::max(1,y1 - y0), dy02 = std::max(1,y2 - y0), dy12 = std::max(1,y2 - y1),
        cy0 = cimg::cut(y0,0,h1), cy2 = cimg::cut(y2,0,h1),
        hdy01 = dy01*cimg::sign(dx01)/2, hdy02 = dy02*cimg::sign(dx02)/2, hdy12 = dy12*cimg::sign(dx12)/2;
      const float
        diz01 = iz1 - iz0, diz02 = iz2 - iz0, diz12 = iz2 - iz1,
        txz0 = tx0*iz0, txz1 = tx1*iz1, txz2 = tx2*iz2,
        tyz0 = ty0*iz0, tyz1 = ty1*iz1, tyz2 = ty2*iz2,
        dtxz01 = txz1 - txz0, dtxz02 = txz2 - txz0, dtxz12 = txz2 - txz1,
        dtyz01 = tyz1 - tyz0, dtyz02 = tyz2 - tyz0, dtyz12 = tyz2 - tyz1,
        lxz0 = lx0*iz0, lxz1 = lx1*iz1, lxz2 = lx2*iz2,
        lyz0 = ly0*iz0, lyz1 = ly1*iz1, lyz2 = ly2*iz2,
        dlxz01 = lxz1 - lxz0, dlxz02 = lxz2 - lxz0, dlxz12 = lxz2 - lxz1,
        dlyz01 = lyz1 - lyz0, dlyz02 = lyz2 - lyz0, dlyz12 = lyz2 - lyz1;

      const ulongT
        twhd = (ulongT)texture._width*texture._height*texture._depth,
        lwhd = (ulongT)light._width*light._height*light._depth;
      cimg_init_scanline(opacity);

      for (int y = cy0; y<=cy2; ++y) {
        const int yy0 = y - y0, yy1 = y - y1;
        int
          xm = y<y1?x0 + (dx01*yy0 + hdy01)/dy01:x1 + (dx12*yy1 + hdy12)/dy12,
          xM = x0 + (dx02*yy0 + hdy02)/dy02;
        float
          izm = y<y1?(iz0 + diz01*yy0/dy01):(iz1 + diz12*yy1/dy12),
          izM = iz0 + diz02*yy0/dy02,
          txzm = y<y1?(txz0 + dtxz01*yy0/dy01):(txz1 + dtxz12*yy1/dy12),
          txzM = txz0 + dtxz02*yy0/dy02,
          tyzm = y<y1?(tyz0 + dtyz01*yy0/dy01):(tyz1 + dtyz12*yy1/dy12),
          tyzM = tyz0 + dtyz02*yy0/dy02,
          lxzm = y<y1?(lxz0 + dlxz01*yy0/dy01):(lxz1 + dlxz12*yy1/dy12),
          lxzM = lxz0 + dlxz02*yy0/dy02,
          lyzm = y<y1?(lyz0 + dlyz01*yy0/dy01):(lyz1 + dlyz12*yy1/dy12),
          lyzM = lyz0 + dlyz02*yy0/dy02;
        if (xm>xM) cimg::swap(xm,xM,izm,izM,txzm,txzM,tyzm,tyzM,lxzm,lxzM,lyzm,lyzM);
        if (xM>=0 || xm<=w1) {
          const int
            cxm = cimg::cut(xm,0,w1),
            cxM = cimg::cut(xM,0,w1);
          T *ptrd = data(cxm,y);
          tz *ptrz = zbuffer.data(cxm,y);
          const int dxmM = std::max(1,xM - xm);
          const float
            dizmM = izM - izm,
            dtxzmM = txzM - txzm, dtyzmM = tyzM - tyzm,
            dlxzmM = lxzM - lxzm, dlyzmM = lyzM - lyzm;

          for (int x = cxm; x<cxM; ++x) {
            const int xxm = x - xm;
            const float iz = izm + dizmM*xxm/dxmM;
            if (iz>=*ptrz) {
              *ptrz = (tz)iz;
              const float
                txz = txzm + dtxzmM*xxm/dxmM,
                tyz = tyzm + dtyzmM*xxm/dxmM,
                lxz = lxzm + dlxzmM*xxm/dxmM,
                lyz = lyzm + dlyzmM*xxm/dxmM;
              const int
                tx = (int)cimg::round(txz/iz),
                ty = (int)cimg::round(tyz/iz),
                lx = (int)cimg::round(lxz/iz),
                ly = (int)cimg::round(lyz/iz);
              const tc *const color = &texture._atXY(tx,ty);
              const tl *const lig = &light._atXY(lx,ly);
              cimg_forC(*this,c) {
                const tc col = color[c*twhd];
                const float cbs = cimg::cut((float)lig[c*lwhd],0,2);
                const Tfloat val = cbs<=1?cbs*col:(2 - cbs)*col + (cbs - 1)*_sc_maxval;
                ptrd[c*_sc_whd] = (T)(opacity>=1?val:val*_sc_nopacity + ptrd[c*_sc_whd]*_sc_copacity);
              }
            }
            ++ptrd; ++ptrz;
          }
        }
      }
      return *this;