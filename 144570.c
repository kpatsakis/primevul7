    template<typename t>
    bool is_sameXZC(const CImg<t>& img) const {
      return is_sameXZC(img._width,img._depth,img._spectrum);