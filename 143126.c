
    inline double rand(const double val_min, const double val_max) {
      cimg::mutex(4);
      const double res = cimg::rand(val_min,val_max,&cimg::rng());
      cimg::mutex(4,0);
      return res;