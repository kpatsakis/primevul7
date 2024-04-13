    template<typename t>
    CImg<T>& operator/=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr / value,32768);
      return *this;