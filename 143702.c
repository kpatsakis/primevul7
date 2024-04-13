    //! Return a reference to the last pixel value \const.
    const T& back() const {
      return *(_data + size() - 1);