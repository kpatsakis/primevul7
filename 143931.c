    template<typename t>
    bool is_sameXYZC(const CImg<t>& img) const {
      return is_sameXYZC(img._width,img._height,img._depth,img._spectrum);