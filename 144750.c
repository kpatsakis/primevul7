
    inline double grand(cimg_ulong *const p_rng) {
      double x1, w;
      do {
        const double x2 = cimg::rand(-1,1,p_rng);
        x1 = cimg::rand(-1,1,p_rng);
        w = x1*x1 + x2*x2;
      } while (w<=0 || w>=1.);
      return x1*std::sqrt((-2*std::log(w))/w);