    template<typename t>
    CImg<T>& distance_dijkstra(const T& value, const CImg<t>& metric,
                               const bool is_high_connectivity=false) {
      return get_distance_dijkstra(value,metric,is_high_connectivity).move_to(*this);