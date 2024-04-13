    //! Permute axes order \newinstance.
    CImg<T> get_permute_axes(const char *const order) const {
      const T foo = (T)0;
      return _permute_axes(order,foo);