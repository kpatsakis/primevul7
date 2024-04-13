    //! Return pointer to the pos-th image of the list \const.
    const CImg<T> *data(const unsigned int l) const {
      return _data + l;