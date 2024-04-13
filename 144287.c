    template<typename tf>
    static CImg<floatT> isosurface3d(CImgList<tf>& primitives, const char *const expression, const float isovalue,
                                     const float x0, const float y0, const float z0,
                                     const float x1, const float y1, const float z1,
                                     const int dx=32, const int dy=32, const int dz=32) {
      const _functor3d_expr func(expression);
      return isosurface3d(primitives,func,isovalue,x0,y0,z0,x1,y1,z1,dx,dy,dz);