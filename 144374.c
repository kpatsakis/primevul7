    // [internal] Replace possibly malicious characters for commands to be called by system() by their escaped version.
    CImg<T>& _system_strescape() {
#define cimg_system_strescape(c,s) case c : if (p!=ptrs) CImg<T>(ptrs,(unsigned int)(p-ptrs),1,1,1,false).\
      move_to(list); \
      CImg<T>(s,(unsigned int)std::strlen(s),1,1,1,false).move_to(list); ptrs = p + 1; break
      CImgList<T> list;
      const T *ptrs = _data;
      cimg_for(*this,p,T) switch ((int)*p) {
        cimg_system_strescape('\\',"\\\\");
        cimg_system_strescape('\"',"\\\"");
        cimg_system_strescape('!',"\"\\!\"");
        cimg_system_strescape('`',"\\`");
        cimg_system_strescape('$',"\\$");
      }
      if (ptrs<end()) CImg<T>(ptrs,(unsigned int)(end()-ptrs),1,1,1,false).move_to(list);
      return (list>'x').move_to(*this);