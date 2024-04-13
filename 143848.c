    template<typename t>
    bool is_sameXY(const CImg<t>& img) const {
      return is_sameXY(img._width,img._height);