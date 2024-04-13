    //! Return the factorial of n
    inline double factorial(const int n) {
      if (n<0) return cimg::type<double>::nan();
      if (n<2) return 1;
      double res = 2;
      for (int i = 3; i<=n; ++i) res*=i;
      return res;