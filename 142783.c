    //! Return a reference to the last image of the list \const.
    CImg<T>& back() {
      return *(_data + _width - 1);