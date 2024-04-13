    template<typename t>
    bool is_sameC(const CImg<t>& img) const {
      return is_sameC(img._spectrum);