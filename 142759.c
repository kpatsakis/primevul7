    //! Convert pixel values from RGB to HSL color spaces.
    CImg<T>& RGBtoHSL() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "RGBtoHSL(): Instance is not a RGB image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          R = (Tfloat)p1[N],
          G = (Tfloat)p2[N],
          B = (Tfloat)p3[N],
          m = cimg::min(R,G,B),
          M = cimg::max(R,G,B),
          C = M - m,
          H = 60*(C==0?0:M==R?cimg::mod((G - B)/C,(Tfloat)6):M==G?(B - R)/C + 2:(R - G)/C + 4),
          L = 0.5f*(m + M)/255,
          S = L==1 || L==0?0:C/(1 - cimg::abs(2*L - 1))/255;
        p1[N] = (T)H;
        p2[N] = (T)S;
        p3[N] = (T)L;
      }
      return *this;