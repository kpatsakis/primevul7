      ~_functor4d_streamline3d_oriented() { delete pI; }
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
#define _cimg_vecalign3d(i,j,k) if (I(i,j,k,0)*I(0,0,0,0) + I(i,j,k,1)*I(0,0,0,1) + I(i,j,k,2)*I(0,0,0,2)<0) { \
  I(i,j,k,0) = -I(i,j,k,0); I(i,j,k,1) = -I(i,j,k,1); I(i,j,k,2) = -I(i,j,k,2); }
        int
          xi = (int)x - (x>=0?0:1), nxi = xi + 1,
          yi = (int)y - (y>=0?0:1), nyi = yi + 1,
          zi = (int)z - (z>=0?0:1), nzi = zi + 1;
        const float
          dx = x - xi,
          dy = y - yi,
          dz = z - zi;
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
          if (zi<0) zi = 0;
          if (nzi<0) nzi = 0;
          if (zi>=ref.depth()) zi = ref.depth() - 1;
          if (nzi>=ref.depth()) nzi = ref.depth() - 1;
          I(0,0,0,0) = (float)ref(xi,yi,zi,0); I(0,0,0,1) = (float)ref(xi,yi,zi,1);
          I(0,0,0,2) = (float)ref(xi,yi,zi,2); I(1,0,0,0) = (float)ref(nxi,yi,zi,0);
          I(1,0,0,1) = (float)ref(nxi,yi,zi,1); I(1,0,0,2) = (float)ref(nxi,yi,zi,2);
          I(1,1,0,0) = (float)ref(nxi,nyi,zi,0); I(1,1,0,1) = (float)ref(nxi,nyi,zi,1);
          I(1,1,0,2) = (float)ref(nxi,nyi,zi,2); I(0,1,0,0) = (float)ref(xi,nyi,zi,0);
          I(0,1,0,1) = (float)ref(xi,nyi,zi,1); I(0,1,0,2) = (float)ref(xi,nyi,zi,2);
          I(0,0,1,0) = (float)ref(xi,yi,nzi,0); I(0,0,1,1) = (float)ref(xi,yi,nzi,1);
          I(0,0,1,2) = (float)ref(xi,yi,nzi,2); I(1,0,1,0) = (float)ref(nxi,yi,nzi,0);
          I(1,0,1,1) = (float)ref(nxi,yi,nzi,1);  I(1,0,1,2) = (float)ref(nxi,yi,nzi,2);
          I(1,1,1,0) = (float)ref(nxi,nyi,nzi,0); I(1,1,1,1) = (float)ref(nxi,nyi,nzi,1);
          I(1,1,1,2) = (float)ref(nxi,nyi,nzi,2); I(0,1,1,0) = (float)ref(xi,nyi,nzi,0);
          I(0,1,1,1) = (float)ref(xi,nyi,nzi,1);  I(0,1,1,2) = (float)ref(xi,nyi,nzi,2);
          _cimg_vecalign3d(1,0,0); _cimg_vecalign3d(1,1,0); _cimg_vecalign3d(0,1,0);
          _cimg_vecalign3d(0,0,1); _cimg_vecalign3d(1,0,1); _cimg_vecalign3d(1,1,1); _cimg_vecalign3d(0,1,1);
        }
        return (float)pI->_linear_atXYZ(dx,dy,dz,c);