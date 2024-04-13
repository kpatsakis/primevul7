    template<typename t>
    bool is_sameXYC(const CImg<t>& img) const {
      return is_sameXYC(img._width,img._height,img._spectrum);