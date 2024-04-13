    //! Erode the image by a square structuring element of specified size \newinstance.
    CImg<T> get_erode(const unsigned int s) const {
      return (+*this).erode(s);