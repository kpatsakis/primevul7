
    CImgDisplay& paint(const bool wait_expose=true) {
      if (is_empty()) return *this;
      cimg_lock_display();
      _paint(wait_expose);
      cimg_unlock_display();
      return *this;