
    inline unsigned int prand(const double z) {
      cimg::mutex(4);
      const unsigned int res = cimg::prand(z,&cimg::rng());
      cimg::mutex(4,0);
      return res;