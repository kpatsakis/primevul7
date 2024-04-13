    **/
    CImg<T>& operator--() {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr - 1,524288);
      return *this;