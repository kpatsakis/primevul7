    //! Return the number of permutations of k objects in a set of n objects.
    inline double permutations(const int k, const int n, const bool with_order) {
      if (n<0 || k<0) return cimg::type<double>::nan();
      if (k>n) return 0;
      double res = 1;
      for (int i = n; i>=n - k + 1; --i) res*=i;
      return with_order?res:res/cimg::factorial(k);