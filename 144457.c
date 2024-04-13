    //! Copy an image into the current image instance \specialization.
    CImg<T>& operator=(const CImg<T>& img) {
      return assign(img);