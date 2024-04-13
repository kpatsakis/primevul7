    template<typename t>
    CImg<T>& assign(const t *const values, const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (!values || !siz) return assign();
      assign(size_x,size_y,size_z,size_c);
      const t *ptrs = values; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      return *this;