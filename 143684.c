    //! Return hyperbolic arcosine of a value.
    inline double acosh(const double x) {
#if cimg_use_cpp11==1 && !defined(_MSC_VER)
      return std::acosh(x);
#else
      return std::log(x + std::sqrt(x*x - 1));
#endif