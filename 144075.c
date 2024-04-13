    //! Convert pixel values from xyY pixels to XYZ color spaces.
    CImg<T>& xyYtoXYZ() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "xyYtoXYZ(): Instance is not a xyY image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,4096))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
         px = (Tfloat)p1[N],
         py = (Tfloat)p2[N],
         Y = (Tfloat)p3[N],
         ny = py>0?py:1;
        p1[N] = (T)(px*Y/ny);
        p2[N] = (T)Y;
        p3[N] = (T)((1 - px - py)*Y/ny);
      }
      return *this;