    template<typename t>
    CImg<T> get_rol(const CImg<t>& img) const {
      return (+*this).rol(img);