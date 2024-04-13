
    inline unsigned int _rand() {
      cimg::mutex(4);
      const unsigned int res = cimg::_rand(&cimg::rng());
      cimg::mutex(4,0);
      return res;