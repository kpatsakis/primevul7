    //! Convert pixel values from CMY to RGB color spaces.
    CImg<T>& CMYtoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "CMYtoRGB(): Instance is not a CMY image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,2048))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          C = (Tfloat)p1[N],
          M = (Tfloat)p2[N],
          Y = (Tfloat)p3[N],
          R = 255 - C,
          G = 255 - M,
          B = 255 - Y;
        p1[N] = (T)cimg::cut(R,0,255),
        p2[N] = (T)cimg::cut(G,0,255),
        p3[N] = (T)cimg::cut(B,0,255);
      }
      return *this;