    **/
    const CImg<T>& save_ffmpeg_external(const char *const filename, const unsigned int fps=25,
                                        const char *const codec=0, const unsigned int bitrate=2048) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_ffmpeg_external(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }

      CImgList<T> list;
      get_split('z').move_to(list);
      list.save_ffmpeg_external(filename,fps,codec,bitrate);
      return *this;