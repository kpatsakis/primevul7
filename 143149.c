    //! Load image sequence using FFMPEG's external tool 'ffmpeg' \newinstance.
    static CImg<T> get_load_ffmpeg_external(const char *const filename, const char axis='z', const float align=0) {
      return CImgList<T>().load_ffmpeg_external(filename).get_append(axis,align);