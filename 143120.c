     **/
    CImg<T> operator-() const {
      return CImg<T>(_width,_height,_depth,_spectrum,(T)0)-=*this;