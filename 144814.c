    **/
    CImg<T>& resize(const int size_x, const int size_y=-100,
                    const int size_z=-100, const int size_c=-100,
                    const int interpolation_type=1, const unsigned int boundary_conditions=0,
                    const float centering_x = 0, const float centering_y = 0,
                    const float centering_z = 0, const float centering_c = 0) {
      if (!size_x || !size_y || !size_z || !size_c) return assign();
      const unsigned int
        _sx = (unsigned int)(size_x<0?-size_x*width()/100:size_x),
        _sy = (unsigned int)(size_y<0?-size_y*height()/100:size_y),
        _sz = (unsigned int)(size_z<0?-size_z*depth()/100:size_z),
        _sc = (unsigned int)(size_c<0?-size_c*spectrum()/100:size_c),
        sx = _sx?_sx:1, sy = _sy?_sy:1, sz = _sz?_sz:1, sc = _sc?_sc:1;
      if (sx==_width && sy==_height && sz==_depth && sc==_spectrum) return *this;
      if (is_empty()) return assign(sx,sy,sz,sc,(T)0);
      if (interpolation_type==-1 && sx*sy*sz*sc==size()) {
	_width = sx; _height = sy; _depth = sz; _spectrum = sc;
	return *this;
      }
      return get_resize(sx,sy,sz,sc,interpolation_type,boundary_conditions,
                        centering_x,centering_y,centering_z,centering_c).move_to(*this);