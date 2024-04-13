     **/
    CImg<T>& min(const T& value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,std::min(*ptr,value),65536);
      return *this;