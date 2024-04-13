    **/
    CImg<T>& cut(const T& min_value, const T& max_value) {
      if (is_empty()) return *this;
      const T a = min_value<max_value?min_value:max_value, b = min_value<max_value?max_value:min_value;
      cimg_openmp_for(*this,cimg::cut(*ptr,a,b),32768);
      return *this;