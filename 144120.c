    template<typename t>
    CImg<T>& operator&=(const t value) {
      if (is_empty()) return *this;
      cimg_openmp_for(*this,(ulongT)*ptr & (ulongT)value,32768);
      return *this;