
    inline void srand(const cimg_ulong seed) {
      cimg::mutex(4);
      cimg::rng() = seed;
      cimg::mutex(4,0);