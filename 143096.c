    template<typename t>
    T& min_max(t& max_val) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "min_max(): Empty instance.",
                                    cimg_instance);
      T *ptr_min = _data;
      T min_value = *ptr_min, max_value = min_value;
      cimg_for(*this,ptrs,T) {
        const T val = *ptrs;
        if (val<min_value) { min_value = val; ptr_min = ptrs; }
        if (val>max_value) max_value = val;
      }
      max_val = (t)max_value;
      return *ptr_min;