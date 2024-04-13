    template<typename t>
    CImg<T>& atan2(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return atan2(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)std::atan2((double)*ptrd,(double)*(ptrs++));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)std::atan2((double)*ptrd,(double)*(ptrs++));
      }
      return *this;