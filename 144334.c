    template<typename t>
    bool is_sameZC(const CImg<t>& img) const {
      return is_sameZC(img._depth,img._spectrum);