    //! Invert endianness of all pixel values \newinstance.
    CImg<T> get_invert_endianness() const {
      return (+*this).invert_endianness();