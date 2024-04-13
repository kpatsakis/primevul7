    template<typename t>
    bool is_sameYC(const CImg<t>& img) const {
      return is_sameYC(img._height,img._spectrum);