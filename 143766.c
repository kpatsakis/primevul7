    //! Fill sequentially all pixel values with specified values \newinstance.
    CImg<T> get_fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                     const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11) const {
      return CImg<T>(_width,_height,_depth,_spectrum).fill(val0,val1,val2,val3,val4,val5,val6,val7,val8,val9,val10,
                                                           val11);