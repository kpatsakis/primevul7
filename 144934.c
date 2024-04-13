    **/
    inline void fempty(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException("cimg::fempty(): Specified filename is (null).");
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      if (!file) cimg::fclose(nfile);