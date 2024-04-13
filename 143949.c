    template<typename t>
    bool is_sameXYZ(const CImg<t>& img) const {
      return is_sameXYZ(img._width,img._height,img._depth);