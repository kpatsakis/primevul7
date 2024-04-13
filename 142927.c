    template<typename t>
    CImg<T>& sort(CImg<t>& permutations, const bool is_increasing=true) {
      permutations.assign(_width,_height,_depth,_spectrum);
      if (is_empty()) return *this;
      cimg_foroff(permutations,off) permutations[off] = (t)off;
      return _quicksort(0,size() - 1,permutations,is_increasing,true);