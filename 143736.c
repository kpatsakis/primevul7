    //! Threshold pixel values \newinstance.
    CImg<T> get_threshold(const T& value, const bool soft_threshold=false, const bool strict_threshold=false) const {
      return (+*this).threshold(value,soft_threshold,strict_threshold);