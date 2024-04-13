    template<typename t>
    CImg<T>& max(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return max(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = std::max((T)*(ptrs++),*ptrd);
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = std::max((T)*(ptrs++),*ptrd);
      }
      return *this;