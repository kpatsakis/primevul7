    template<typename T>
    inline T rol(const T& a, const unsigned int n=1) {
      return n?(T)((a<<n)|(a>>((sizeof(T)<<3) - n))):a;