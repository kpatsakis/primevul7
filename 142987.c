    template<typename t>
    CImg<T>& operator>>=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,((longT)*ptr) >> (int)value,65536);
      return *this;