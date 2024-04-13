
    inline double grand() {
      cimg::mutex(4);
      const double res = cimg::grand(&cimg::rng());
      cimg::mutex(4,0);
      return res;