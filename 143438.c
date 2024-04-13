    **/
    CImg<T>& blur(const float sigma_x, const float sigma_y, const float sigma_z,
                  const bool boundary_conditions=true, const bool is_gaussian=false) {
      if (is_empty()) return *this;
      if (is_gaussian) {
        if (_width>1) vanvliet(sigma_x,0,'x',boundary_conditions);
        if (_height>1) vanvliet(sigma_y,0,'y',boundary_conditions);
        if (_depth>1) vanvliet(sigma_z,0,'z',boundary_conditions);
      } else {
        if (_width>1) deriche(sigma_x,0,'x',boundary_conditions);
        if (_height>1) deriche(sigma_y,0,'y',boundary_conditions);
        if (_depth>1) deriche(sigma_z,0,'z',boundary_conditions);
      }
      return *this;