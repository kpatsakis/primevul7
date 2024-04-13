    **/
    CImg<T>& rol(const unsigned int n=1) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::rol(*ptr,n),32768);
      return *this;