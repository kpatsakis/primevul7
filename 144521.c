      ~_functor4d_streamline2d_oriented() { delete pI; }
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
#define _cimg_vecalign2d(i,j) \
        if (I(i,j,0)*I(0,0,0) + I(i,j,1)*I(0,0,1)<0) { I(i,j,0) = -I(i,j,0); I(i,j,1) = -I(i,j,1); }
        int
          xi = (int)x - (x>=0?0:1), nxi = xi + 1,
          yi = (int)y - (y>=0?0:1), nyi = yi + 1,
          zi = (int)z;
        const float
          dx = x - xi,
          dy = y - yi;
        if (c==0) {
          CImg<floatT>& I = *pI;
          if (xi<0) xi = 0;
          if (nxi<0) nxi = 0;
          if (xi>=ref.width()) xi = ref.width() - 1;
          if (nxi>=ref.width()) nxi = ref.width() - 1;
          if (yi<0) yi = 0;
          if (nyi<0) nyi = 0;
          if (yi>=ref.height()) yi = ref.height() - 1;
          if (nyi>=ref.height()) nyi = ref.height() - 1;
          I(0,0,0) = (float)ref(xi,yi,zi,0);   I(0,0,1) = (float)ref(xi,yi,zi,1);
          I(1,0,0) = (float)ref(nxi,yi,zi,0);  I(1,0,1) = (float)ref(nxi,yi,zi,1);
          I(1,1,0) = (float)ref(nxi,nyi,zi,0); I(1,1,1) = (float)ref(nxi,nyi,zi,1);
          I(0,1,0) = (float)ref(xi,nyi,zi,0);  I(0,1,1) = (float)ref(xi,nyi,zi,1);
          _cimg_vecalign2d(1,0); _cimg_vecalign2d(1,1); _cimg_vecalign2d(0,1);
        }
        return c<2?(float)pI->_linear_atXY(dx,dy,0,c):0;