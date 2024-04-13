    template<typename t>
    bool is_sameYZ(const CImg<t>& img) const {
      return is_sameYZ(img._height,img._depth);