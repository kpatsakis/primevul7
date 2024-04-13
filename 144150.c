    // Custom random number generator (allow re-entrance).
    inline cimg_ulong& rng() { // Used as a shared global number for rng
      static cimg_ulong rng = 0xB16B00B5U;
      return rng;