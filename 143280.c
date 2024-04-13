
    const CImg<T>& _save_dlm(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_dlm(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_dlm(): Instance is volumetric, values along Z will be unrolled in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");
      if (_spectrum>1)
        cimg::warn(_cimg_instance
                   "save_dlm(): Instance is multispectral, values along C will be unrolled in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      const T* ptrs = _data;
      cimg_forYZC(*this,y,z,c) {
        cimg_forX(*this,x) std::fprintf(nfile,"%.17g%s",(double)*(ptrs++),(x==width() - 1)?"":",");
        std::fputc('\n',nfile);
      }
      if (!file) cimg::fclose(nfile);
      return *this;