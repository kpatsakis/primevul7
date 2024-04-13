    template<typename t, typename to>
    CImg<T>& distance_dijkstra(const T& value, const CImg<t>& metric, const bool is_high_connectivity,
                               CImg<to>& return_path) {
      return get_distance_dijkstra(value,metric,is_high_connectivity,return_path).move_to(*this);