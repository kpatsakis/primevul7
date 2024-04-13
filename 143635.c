        \param last_frame Index of last image frame to read.
        \param step_frame Step applied between each frame.
        \param yuv2rgb Apply YUV to RGB transformation during reading.
    **/
    CImgList<T>& load_yuv(const char *const filename,
                          const unsigned int size_x, const unsigned int size_y,
                          const unsigned int chroma_subsampling=444,
                          const unsigned int first_frame=0, const unsigned int last_frame=~0U,