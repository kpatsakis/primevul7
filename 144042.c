    **/
    const CImg<T>& save_video(const char *const filename, const unsigned int fps=25,
                              const char *codec=0, const bool keep_open=false) const {
      if (is_empty()) { CImgList<T>().save_video(filename,fps,codec,keep_open); return *this; }
      CImgList<T> list;
      get_split('z').move_to(list);
      list.save_video(filename,fps,codec,keep_open);
      return *this;