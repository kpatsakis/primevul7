    **/
    CImg<T>& sequence(const T& a0, const T& a1) {
      if (is_empty()) return *this;
      const ulongT siz = size() - 1;
      T* ptr = _data;
      if (siz) {
        const double delta = (double)a1 - (double)a0;
        cimg_foroff(*this,l) *(ptr++) = (T)(a0 + delta*l/siz);
      } else *ptr = a0;
      return *this;