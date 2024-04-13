     **/
    T& max() {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max(): Empty instance.",
                                    cimg_instance);
      T *ptr_max = _data;
      T max_value = *ptr_max;
      cimg_for(*this,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      return *ptr_max;