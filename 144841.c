
    inline double _fibonacci(int exp) {
      double
        base = (1 + std::sqrt(5.))/2,
        result = 1/std::sqrt(5.);
      while (exp) {
        if (exp&1) result*=base;
        exp>>=1;
        base*=base;
      }
      return result;