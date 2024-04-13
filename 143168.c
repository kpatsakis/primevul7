    template<typename t>
    CImg<T>& operator*=(const CImg<t>& img) {
      return ((*this)*img).move_to(*this);