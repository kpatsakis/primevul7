    //! Dilate image by a square structuring element of specified size \newinstance.
    CImg<T> get_dilate(const unsigned int s) const {
      return (+*this).dilate(s);