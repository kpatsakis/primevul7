    template<typename t>
    bool is_sameXC(const CImg<t>& img) const {
      return is_sameXC(img._width,img._spectrum);