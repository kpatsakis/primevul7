
    inline double rand(const double val_max=1) {
      cimg::mutex(4);
      const double res = cimg::rand(val_max,&cimg::rng());
      cimg::mutex(4,0);
      return res;