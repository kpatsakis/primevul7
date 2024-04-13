    //! Return hyperbolic arctangent of a value.
    inline double atanh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::atanh(x);
#else
      return 0.5*std::log((1. + x)/(1. - x));
#endif