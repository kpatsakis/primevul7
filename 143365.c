    **/
    CImg<T>& crop(const int x0, const int y0, const int z0, const int c0,
                  const int x1, const int y1, const int z1, const int c1,
                  const unsigned int boundary_conditions=0) {
      return get_crop(x0,y0,z0,c0,x1,y1,z1,c1,boundary_conditions).move_to(*this);