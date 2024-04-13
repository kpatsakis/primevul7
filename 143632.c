    //! Sort pixel values \newinstance.
    CImg<T> get_sort(const bool is_increasing=true, const char axis=0) const {
      return (+*this).sort(is_increasing,axis);