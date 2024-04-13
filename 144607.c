    //! Return minimal path in a graph, using the Dijkstra algorithm \newinstance.
    CImg<Tfloat> get_dijkstra(const unsigned int starting_node, const unsigned int ending_node=~0U) const {
      CImg<uintT> foo;
      return get_dijkstra(starting_node,ending_node,foo);