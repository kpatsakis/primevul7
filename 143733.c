       \param[out] c C-coordinate of the pixel value, if test succeeds.
       \note If true, set coordinates (n,x,y,z,c).
    **/
    template<typename t>
    bool contains(const T& pixel, t& n, t& x, t&y, t& z, t& c) const {