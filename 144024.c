    template<typename t>
    CImg<T>& erode(const CImg<t>& kernel, const bool boundary_conditions=true,
                   const bool is_real=false) {
      if (is_empty() || !kernel) return *this;
      return get_erode(kernel,boundary_conditions,is_real).move_to(*this);