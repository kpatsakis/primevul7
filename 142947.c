    //! Transfer content of an image instance into another one \specialization.
    CImg<T>& move_to(CImg<T>& img) {
      if (_is_shared || img._is_shared) img.assign(*this);
      else swap(img);
      assign();
      return img;