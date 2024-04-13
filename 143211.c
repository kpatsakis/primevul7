    **/
    const CImg<T>& save_pandore(std::FILE *const file, const unsigned int colorspace=0) const {
      return _save_pandore(file,0,colorspace);