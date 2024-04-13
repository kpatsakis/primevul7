#ifdef cimg_use_half
    inline half ror(const half a, const unsigned int n=1) {
      return (half)ror((int)a,n);