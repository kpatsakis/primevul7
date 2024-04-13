    //! Convert pixel values from HSI to RGB color spaces.
    CImg<T>& HSItoRGB() {
      if (_spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "HSItoRGB(): Instance is not a HSI image.",
                                    cimg_instance);

      T *p1 = data(0,0,0,0), *p2 = data(0,0,0,1), *p3 = data(0,0,0,2);
      const longT whd = (longT)width()*height()*depth();
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(whd,256))
      for (longT N = 0; N<whd; ++N) {
        const Tfloat
          H = cimg::mod((Tfloat)p1[N]/60,(Tfloat)6),
          S = (Tfloat)p2[N],
          I = (Tfloat)p3[N],
          Z = 1 - cimg::abs(cimg::mod(H,(Tfloat)2) - 1),
          C = I*S/(1 + Z),
          X = C*Z,
          m = I*(1 - S)/3;
        Tfloat R, G, B;
        switch ((int)H) {
        case 0 : R = C; G = X; B = 0; break;
        case 1 : R = X; G = C; B = 0; break;
        case 2 : R = 0; G = C; B = X; break;
        case 3 : R = 0; G = X; B = C; break;
        case 4 : R = X; G = 0; B = C; break;
        default : R = C; G = 0; B = X;
        }
        p1[N] = (T)((R + m)*3*255);
        p2[N] = (T)((G + m)*3*255);
        p3[N] = (T)((B + m)*3*255);
      }
      return *this;