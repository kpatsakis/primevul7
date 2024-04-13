    }

    //! Return a reference to the minimum pixel value of the instance list and return the minimum value as well \const.
    template<typename t>
    const T& max_min(t& min_val) const {
      bool is_all_empty = true;
      T *ptr_max = 0;
      cimglist_for(*this,l) if (!_data[l].is_empty()) {
        ptr_max = _data[l]._data;
        is_all_empty = false;
        break;
      }
      if (is_all_empty)
        throw CImgInstanceException(_cimglist_instance
                                    "max_min(): %s.",
                                    _data?"List of empty images":"Empty instance",
                                    cimglist_instance);
      T min_value = *ptr_max, max_value = min_value;
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_for(img,ptrs,T) {
          const T val = *ptrs;
          if (val>max_value) { max_value = val; ptr_max = ptrs; }
          if (val<min_value) min_value = val;
        }