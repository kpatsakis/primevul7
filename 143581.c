    template<typename t>
    bool operator==(const CImg<t>& img) const {
      typedef _cimg_Tt Tt;
      const ulongT siz = size();
      bool is_equal = true;
      if (siz!=img.size()) return false;
      t *ptrs = img._data + siz;
      for (T *ptrd = _data + siz; is_equal && ptrd>_data; is_equal = ((Tt)*(--ptrd)==(Tt)*(--ptrs))) {}
      return is_equal;