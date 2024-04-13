    //! Fill sequentially all pixel values with specified values \overloading.
    CImg<T>& fill(const T& val0, const T& val1, const T& val2, const T& val3, const T& val4, const T& val5,
                  const T& val6, const T& val7, const T& val8, const T& val9, const T& val10, const T& val11) {
      if (is_empty()) return *this;
      T *ptrd, *ptre = end() - 11;
      for (ptrd = _data; ptrd<ptre; ) {
        *(ptrd++) = val0; *(ptrd++) = val1; *(ptrd++) = val2; *(ptrd++) = val3; *(ptrd++) = val4; *(ptrd++) = val5;
        *(ptrd++) = val6; *(ptrd++) = val7; *(ptrd++) = val8; *(ptrd++) = val9; *(ptrd++) = val10; *(ptrd++) = val11;
      }
      ptre+=11;
      switch (ptre - ptrd) {
      case 11 : *(--ptre) = val10; // fallthrough
      case 10 : *(--ptre) = val9; // fallthrough
      case 9 : *(--ptre) = val8; // fallthrough
      case 8 : *(--ptre) = val7; // fallthrough
      case 7 : *(--ptre) = val6; // fallthrough
      case 6 : *(--ptre) = val5; // fallthrough
      case 5 : *(--ptre) = val4; // fallthrough
      case 4 : *(--ptre) = val3; // fallthrough
      case 3 : *(--ptre) = val2; // fallthrough
      case 2 : *(--ptre) = val1; // fallthrough
      case 1 : *(--ptre) = val0; // fallthrough
      }
      return *this;