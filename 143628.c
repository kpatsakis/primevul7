    template<typename t>
    CImg<T> get_dijkstra(const unsigned int starting_node, const unsigned int ending_node,
                         CImg<t>& previous_node) const {
      if (_width!=_height || _depth!=1 || _spectrum!=1)
        throw CImgInstanceException(_cimg_instance
                                    "dijkstra(): Instance is not a graph adjacency matrix.",
                                    cimg_instance);

      return dijkstra(*this,_width,starting_node,ending_node,previous_node);