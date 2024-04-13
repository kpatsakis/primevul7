    //! Round pixel values \newinstance.
    CImg<T> get_round(const double y=1, const unsigned int rounding_type=0) const {
      return (+*this).round(y,rounding_type);