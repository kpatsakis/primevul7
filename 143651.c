    template<typename T>
    inline T minmod(const T& a, const T& b) {
      return a*b<=0?0:(a>0?(a<b?a:b):(a<b?b:a));