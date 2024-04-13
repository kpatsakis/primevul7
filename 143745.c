    //! Return base-2 logarithm of a value.
    inline double log2(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::log2(x);
#else
      const double base2 = std::log(2.);
      return std::log(x)/base2;
#endif