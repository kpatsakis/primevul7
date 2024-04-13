    **/
    CImg<T>& histogram(const unsigned int nb_levels, const T& min_value, const T& max_value) {
      return get_histogram(nb_levels,min_value,max_value).move_to(*this);