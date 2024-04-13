    **/
    CImg<T>& load_video(const char *const filename,
                        const unsigned int first_frame=0, const unsigned int last_frame=~0U,
                        const unsigned int step_frame=1,
                        const char axis='z', const float align=0) {
      return get_load_video(filename,first_frame,last_frame,step_frame,axis,align).move_to(*this);