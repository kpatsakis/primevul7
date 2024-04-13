    template<typename t>
    CImg<T>& assign(const CImg<t>& img, const char *const dimensions, const T& value) {
      return assign(img,dimensions).fill(value);