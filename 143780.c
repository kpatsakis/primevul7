    template<typename t>
    CImg<T>& operator/=(const CImg<t>& img) {
      return (*this*img.get_invert()).move_to(*this);