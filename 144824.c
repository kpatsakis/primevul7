    **/
    CImg<T>& load_ffmpeg_external(const char *const filename, const char axis='z', const float align=0) {
      return get_load_ffmpeg_external(filename,axis,align).move_to(*this);