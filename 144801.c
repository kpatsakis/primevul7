     **/
    CImg<T>& max(const T& value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,std::max(*ptr,value),65536);
      return *this;