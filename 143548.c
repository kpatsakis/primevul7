    template<typename t>
    bool operator!=(const CImg<t>& img) const {
      return !((*this)==img);