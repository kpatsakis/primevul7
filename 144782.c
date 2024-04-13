    //! Save image as a .cpp source file \overloading.
    const CImg<T>& save_cpp(std::FILE *const file) const {
      return _save_cpp(file,0);