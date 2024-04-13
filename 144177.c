     **/
    CImg<T> operator++(int) {
      const CImg<T> copy(*this,false);
      ++*this;
      return copy;