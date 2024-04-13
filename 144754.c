    **/
    CImg<T>& ror(const unsigned int n=1) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::ror(*ptr,n),32768);
      return *this;