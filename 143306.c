    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3);