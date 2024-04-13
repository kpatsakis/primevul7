
    static float _cimg_lanczos(const float x) {
      if (x<=-2 || x>=2) return 0;
      const float a = (float)cimg::PI*x, b = 0.5f*a;
      return (float)(x?std::sin(a)*std::sin(b)/(a*b):1);