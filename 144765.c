
    //! Return a reference to the minimum pixel value of the instance list.
    /**
    **/
    T& min() {
      bool is_all_empty = true;
      T *ptr_min = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_min = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_min;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];