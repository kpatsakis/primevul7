    **/
    CImg<T>& fill(const T& val0, const T& val1) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 1;
      for (ptrd = _data; ptrd<ptre; ) { *(ptrd++) = val0; *(ptrd++) = val1; }
      if (ptrd!=ptre + 1) *(ptrd++) = val0;
      return *this;