    template<typename t>
    CImg<T>& fill(const CImg<t>& values, const bool repeat_values=true) {
      if (is_empty() || !values) return *this;
      T *ptrd = _data, *ptre = ptrd + size();
      for (t *ptrs = values._data, *ptrs_end = ptrs + values.size(); ptrs<ptrs_end && ptrd<ptre; ++ptrs)
        *(ptrd++) = (T)*ptrs;
      if (repeat_values && ptrd<ptre) for (T *ptrs = _data; ptrd<ptre; ++ptrs) *(ptrd++) = *ptrs;
      return *this;