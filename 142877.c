    //! Return a CImg<T>::iterator pointing next to the last pixel value \const.
    const_iterator end() const {
      return _data + size();