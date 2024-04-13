    template<typename t>
    CImg<t>& move_to(CImg<t>& img) {
      img.assign(*this);
      assign();
      return img;