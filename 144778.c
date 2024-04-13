    template<typename t>
    bool is_sameYZC(const CImg<t>& img) const {
      return is_sameYZC(img._height,img._depth,img._spectrum);