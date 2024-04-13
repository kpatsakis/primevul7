    // (Precise up to n = 78, less precise for n>78).
    inline double fibonacci(const int n) {
      if (n<0) return cimg::type<double>::nan();
      if (n<3) return 1;
      if (n<11) {
        cimg_uint64 fn1 = 1, fn2 = 1, fn = 0;
        for (int i = 3; i<=n; ++i) { fn = fn1 + fn2; fn2 = fn1; fn1 = fn; }
        return (double)fn;
      }
      if (n<75) // precise up to n = 74, faster than the integer calculation above for n>10
        return (double)((cimg_uint64)(_fibonacci(n) + 0.5));

      if (n<94) { // precise up to n = 78, less precise for n>78 up to n = 93, overflows for n>93
        cimg_uint64
          fn1 = (cimg_uint64)1304969544928657ULL,
          fn2 = (cimg_uint64)806515533049393ULL,
          fn = 0;
        for (int i = 75; i<=n; ++i) { fn = fn1 + fn2; fn2 = fn1; fn1 = fn; }
        return (double)fn;
      }
      return _fibonacci(n); // Not precise, but better than the wrong overflowing calculation