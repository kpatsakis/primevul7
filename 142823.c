#ifdef cimg_use_half
    inline half rol(const half a, const unsigned int n=1) {
      return (half)rol((int)a,n);