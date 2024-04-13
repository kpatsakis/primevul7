    template<typename t>
    T& max_min(t& min_val) {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max_min(): Empty instance.",
                                    cimg_instance);
      T *ptr_max = _data;
      T max_value = *ptr_max, min_value = max_value;
      cimg_for(*this,ptrs,T) {
        const T val = *ptrs;
        if (val>max_value) { max_value = val; ptr_max = ptrs; }
        if (val<min_value) min_value = val;
      }
      min_val = (t)min_value;
      return *ptr_max;