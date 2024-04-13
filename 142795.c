    //! Return hyperbolic arcsine of a value.
    inline double asinh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::asinh(x);
#else
      return std::log(x + std::sqrt(x*x + 1));
#endif