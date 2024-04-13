    template<typename t>
    CImg<T>& distance_eikonal(const T& value, const CImg<t>& metric) {
      return get_distance_eikonal(value,metric).move_to(*this);