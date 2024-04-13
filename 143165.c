    //! Return a shared-memory version of the image instance.
    CImg<T> get_shared() {
      return CImg<T>(_data,_width,_height,_depth,_spectrum,true);