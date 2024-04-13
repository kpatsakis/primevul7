       \param[out] n Index of image in the list, if test succeeds.
       \note If true, returns the position (n) of the image in the list.
    **/
    template<typename t>
    bool contains(const CImg<T>& img, t& n) const {
      if (is_empty()) return false;