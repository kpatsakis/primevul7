      return _load_cimg(file,0,n0,n1,x0,y0,z0,c0,x1,y1,z1,c1);
    }

    //! Load a sub-image list from a (non compressed) .cimg file \newinstance.
    static CImgList<T> get_load_cimg(std::FILE *const file,
                                     const unsigned int n0, const unsigned int n1,
                                     const unsigned int x0, const unsigned int y0,
                                     const unsigned int z0, const unsigned int c0,