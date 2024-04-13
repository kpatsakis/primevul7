    template<typename t>
    CImg<T> operator|(const t value) const {
      return (+*this)|=value;