    template<typename tf, typename t>
    static CImg<T> dijkstra(const tf& distance, const unsigned int nb_nodes,
                            const unsigned int starting_node, const unsigned int ending_node=~0U) {
      CImg<uintT> foo;
      return dijkstra(distance,nb_nodes,starting_node,ending_node,foo);