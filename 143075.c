    **/
    CImg<T>& blur_box(const float boxsize_x, const float boxsize_y, const float boxsize_z,
                      const bool boundary_conditions=true,
                      const unsigned int nb_iter=1) {
      if (is_empty()) return *this;
      if (_width>1) boxfilter(boxsize_x,0,'x',boundary_conditions,nb_iter);
      if (_height>1) boxfilter(boxsize_y,0,'y',boundary_conditions,nb_iter);
      if (_depth>1) boxfilter(boxsize_z,0,'z',boundary_conditions,nb_iter);
      return *this;