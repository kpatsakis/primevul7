     **/
    CImg<T>& invert_endianness() {
      cimg::invert_endianness(_data,size());
      return *this;