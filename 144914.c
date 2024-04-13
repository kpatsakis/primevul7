    template<typename t>
    CImg<T>& discard(const CImg<t>& values, const char axis=0) {
      if (is_empty() || !values) return *this;
      return get_discard(values,axis).move_to(*this);