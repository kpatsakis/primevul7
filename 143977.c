    template<typename t>
    CImg<T>& operator%=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,cimg::mod(*ptr,(T)value),16384);
      return *this;