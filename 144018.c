    **/
    CImg<T> operator+() const {
      return CImg<T>(*this,false);