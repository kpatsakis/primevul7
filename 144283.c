    **/
    CImgDisplay& wait(const unsigned int milliseconds) {
      cimg::wait(milliseconds,&_timer);
      return *this;