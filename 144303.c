    template<typename t>
    CImg<T> operator>>(const CImg<t>& img) const {
      return (+*this)>>=img;