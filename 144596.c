    //! Return minimal path in a graph, using the Dijkstra algorithm.
    CImg<T>& dijkstra(const unsigned int starting_node, const unsigned int ending_node=~0U) {
      return get_dijkstra(starting_node,ending_node).move_to(*this);