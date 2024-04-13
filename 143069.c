    template<typename t>
    CImg<T> get_watershed(const CImg<t>& priority, const bool is_high_connectivity=false) const {
      return (+*this).watershed(priority,is_high_connectivity);