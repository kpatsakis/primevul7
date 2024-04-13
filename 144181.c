    //! Load image sequence from a YUV file \overloading.
    CImg<T>& load_yuv(std::FILE *const file,
                      const unsigned int size_x, const unsigned int size_y=1,
                      const unsigned int chroma_subsampling=444,
                      const unsigned int first_frame=0, const unsigned int last_frame=~0U,
		      const unsigned int step_frame=1, const bool yuv2rgb=true, const char axis='z') {
      return get_load_yuv(file,size_x,size_y,chroma_subsampling,
                          first_frame,last_frame,step_frame,yuv2rgb,axis).move_to(*this);