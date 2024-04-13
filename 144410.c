
    const CImg<T>& _save_cpp(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_cpp(): Specified filename is (null).",
                                    cimg_instance);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"w");
      CImg<charT> varname(1024); *varname = 0;
      if (filename) cimg_sscanf(cimg::basename(filename),"%1023[a-zA-Z0-9_]",varname._data);
      if (!*varname) cimg_snprintf(varname,varname._width,"unnamed");
      std::fprintf(nfile,
                   "/* Define image '%s' of size %ux%ux%ux%u and type '%s' */\n"
                   "%s data_%s[] = { %s\n  ",
                   varname._data,_width,_height,_depth,_spectrum,pixel_type(),pixel_type(),varname._data,
                   is_empty()?"};":"");
      if (!is_empty()) for (ulongT off = 0, siz = size() - 1; off<=siz; ++off) {
        std::fprintf(nfile,cimg::type<T>::format(),cimg::type<T>::format((*this)[off]));
        if (off==siz) std::fprintf(nfile," };\n");
        else if (!((off + 1)%16)) std::fprintf(nfile,",\n  ");
        else std::fprintf(nfile,", ");
      }
      if (!file) cimg::fclose(nfile);
      return *this;