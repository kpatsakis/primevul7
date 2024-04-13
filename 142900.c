    //! Return the sinc of a given value.
    inline double sinc(const double x) {
      return x?std::sin(x)/x:1;