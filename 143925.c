    //! Return a reference to the maximum pixel value \const.
    const T& max() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "max(): Empty instance.",
                                    cimg_instance);
      const T *ptr_max = _data;
      T max_value = *ptr_max;
      cimg_for(*this,ptrs,T) if (*ptrs>max_value) max_value = *(ptr_max=ptrs);
      return *ptr_max;