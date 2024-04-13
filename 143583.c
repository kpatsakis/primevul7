    **/
    CImg<T>& rotate(const float angle, const float cx, const float cy,
                    const unsigned int interpolation, const unsigned int boundary_conditions=0) {
      return get_rotate(angle,cx,cy,interpolation,boundary_conditions).move_to(*this);