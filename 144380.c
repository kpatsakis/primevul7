    template<typename T>
    inline cimg_ulong nearest_pow2(const T& x) {
      cimg_ulong i = 1;
      while (x>i) i<<=1;
      return i;