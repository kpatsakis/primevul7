    //! Implicitely cast an image into a \c T* \const.
    operator const T*() const {
      return _data;