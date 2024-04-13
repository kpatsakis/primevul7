    //! Return a reference to the minimum pixel value \const.
    const T& min() const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "min(): Empty instance.",
                                    cimg_instance);
      const T *ptr_min = _data;
      T min_value = *ptr_min;
      cimg_for(*this,ptrs,T) if (*ptrs<min_value) min_value = *(ptr_min=ptrs);
      return *ptr_min;