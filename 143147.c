    template<typename t>
    CImg<T>& dijkstra(const unsigned int starting_node, const unsigned int ending_node,
                      CImg<t>& previous_node) {
      return get_dijkstra(starting_node,ending_node,previous_node).move_to(*this);