    template<typename t>
    CImg<T> get_sort(CImg<t>& permutations, const bool is_increasing=true) const {
      return (+*this).sort(permutations,is_increasing);