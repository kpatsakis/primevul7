    template<typename t>
    bool is_sameXZ(const CImg<t>& img) const {
      return is_sameXZ(img._width,img._depth);