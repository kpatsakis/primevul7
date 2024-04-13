    template<typename t>
    bool operator==(const t value) const {
      if (is_empty()) return false;
      typedef _cimg_Tt Tt;
      bool is_equal = true;
      for (T *ptrd = _data + size(); is_equal && ptrd>_data; is_equal = ((Tt)*(--ptrd)==(Tt)value)) {}
      return is_equal;