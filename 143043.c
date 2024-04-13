    //! Wait for any user event occuring on the current display.
    CImgDisplay& wait() {
      wait(*this);
      return *this;