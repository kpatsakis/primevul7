    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 4;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4;
      }
      ptre+=4;
      switch (ptre - ptrd) {
      case 4 : *(--ptre) = val3; // fallthrough
      case 3 : *(--ptre) = val2; // fallthrough
      case 2 : *(--ptre) = val1; // fallthrough
      case 1 : *(--ptre) = val0; // fallthrough
      }
      return *this;