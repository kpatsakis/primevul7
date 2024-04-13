    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 2;
      for (ptrd = _data; ptrd<ptre; ) { *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; }
      ptre+=2;
      switch (ptre - ptrd) {
      case 2 : *(--ptre) = val1; // fallthrough
      case 1 : *(--ptre) = val0; // fallthrough
      }
      return *this;