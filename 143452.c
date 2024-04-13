    //! Uniformly quantize pixel values \newinstance.
    CImg<T> get_quantize(const unsigned int n, const bool keep_range=true) const {
      return (+*this).quantize(n,keep_range);