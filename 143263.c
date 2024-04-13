    template<typename t>
    CImg<Tfloat> get_distance(const T& value, const CImg<t>& metric_mask) const {
      return CImg<Tfloat>(*this,false).distance(value,metric_mask);