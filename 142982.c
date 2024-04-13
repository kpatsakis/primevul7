
    inline void srand() {
      cimg::mutex(4);
      cimg::srand(&cimg::rng());
      cimg::mutex(4,0);