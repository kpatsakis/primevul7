    template<typename t>
    CImg<T> get_ror(const CImg<t>& img) const {
      return (+*this).ror(img);