    template<typename T>
    inline T& invert_endianness(T& a) {
      invert_endianness(&a,1);
      return a;