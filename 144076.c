    template<typename tf, typename tfunc>
    static CImg<floatT> elevation3d(CImgList<tf>& primitives, const tfunc& func,
                                    const float x0, const float y0, const float x1, const float y1,
                                    const int size_x=256, const int size_y=256) {
      const float
        nx0 = x0<x1?x0:x1, ny0 = y0<y1?y0:y1,
        nx1 = x0<x1?x1:x0, ny1 = y0<y1?y1:y0;
      const unsigned int
        _nsize_x = (unsigned int)(size_x>=0?size_x:(nx1-nx0)*-size_x/100),
        nsize_x = _nsize_x?_nsize_x:1, nsize_x1 = nsize_x - 1,
        _nsize_y = (unsigned int)(size_y>=0?size_y:(ny1-ny0)*-size_y/100),
        nsize_y = _nsize_y?_nsize_y:1, nsize_y1 = nsize_y - 1;
      if (nsize_x<2 || nsize_y<2)
        throw CImgArgumentException("CImg<%s>::elevation3d(): Invalid specified size (%d,%d).",
                                    pixel_type(),
                                    nsize_x,nsize_y);

      CImg<floatT> vertices(nsize_x*nsize_y,3);
      floatT *ptr_x = vertices.data(0,0), *ptr_y = vertices.data(0,1), *ptr_z = vertices.data(0,2);
      for (unsigned int y = 0; y<nsize_y; ++y) {
        const float Y = ny0 + y*(ny1-ny0)/nsize_y1;
        for (unsigned int x = 0; x<nsize_x; ++x) {
          const float X = nx0 + x*(nx1-nx0)/nsize_x1;
          *(ptr_x++) = (float)x;
          *(ptr_y++) = (float)y;
          *(ptr_z++) = (float)func(X,Y);
        }
      }
      primitives.assign(nsize_x1*nsize_y1,1,4);
      for (unsigned int p = 0, y = 0; y<nsize_y1; ++y) {
        const unsigned int yw = y*nsize_x;
        for (unsigned int x = 0; x<nsize_x1; ++x) {
          const unsigned int xpyw = x + yw, xpyww = xpyw + nsize_x;
          primitives[p++].fill(xpyw,xpyww,xpyww + 1,xpyw + 1);
        }
      }
      return vertices;