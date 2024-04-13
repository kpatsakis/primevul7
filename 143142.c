    //! Load image sequence from a video file, using OpenCV library \newinstance.
    static CImg<T> get_load_video(const char *const filename,
                                  const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                                  const unsigned int step_frame=1,
                                  const char axis='z', const float align=0) {
      return CImgList<T>().load_video(filename,first_frame,last_frame,step_frame).get_append(axis,align);