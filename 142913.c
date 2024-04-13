    template<typename t>
    CImg<T>& set_matrix_at(const CImg<t>& mat, const unsigned int x=0, const unsigned int y=0, const unsigned int z=0) {
      return set_vector_at(mat,x,y,z);