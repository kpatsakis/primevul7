    template<typename t>
    CImg<T>& operator-=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,*ptr - value,524288);
      return *this;