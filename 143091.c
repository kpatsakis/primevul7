                       first_frame,last_frame,step_frame,yuv2rgb);
    }

    //! Load a list from a YUV image sequence file \newinstance.
    static CImgList<T> get_load_yuv(const char *const filename,
                                    const unsigned int size_x, const unsigned int size_y=1,
                                    const unsigned int chroma_subsampling=444,
                                    const unsigned int first_frame=0, const unsigned int last_frame=~0U,