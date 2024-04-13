    //! Return a shared-memory version of the image instance \const.
    const CImg<T> get_shared() const {
      return CImg<T>(_data,_width,_height,_depth,_spectrum,true);