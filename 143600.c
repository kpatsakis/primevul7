    template<typename t>
    CImg<T>& pow(const CImg<t>& img) {
      const ulongT siz = size(), isiz = img.size();
      if (siz && isiz) {
        if (is_overlapped(img)) return pow(+img);
        T *ptrd = _data, *const ptre = _data + siz;
        if (siz>isiz) for (ulongT n = siz/isiz; n; --n)
          for (const t *ptrs = img._data, *ptrs_end = ptrs + isiz; ptrs<ptrs_end; ++ptrd)
            *ptrd = (T)std::pow((double)*ptrd,(double)(*(ptrs++)));
        for (const t *ptrs = img._data; ptrd<ptre; ++ptrd) *ptrd = (T)std::pow((double)*ptrd,(double)(*(ptrs++)));
      }
      return *this;