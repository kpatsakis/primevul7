    template<typename t>
    CImg<Tfloat> get_distance_dijkstra(const T& value, const CImg<t>& metric,
                                       const bool is_high_connectivity=false) const {
      CImg<T> return_path;
      return get_distance_dijkstra(value,metric,is_high_connectivity,return_path);