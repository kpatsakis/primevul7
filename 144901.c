    // Convert a continuous cv::Mat<uchar> to a CImg<uchar>.
    static CImg<ucharT> _cvmat2cimg(const cv::Mat &src) {
      if (src.channels()==1) return CImg<ucharT>(src.ptr(),src.cols,src.rows,1,1);
      else if (src.channels()==3) { // BGR
        CImg<ucharT> res(src.cols,src.rows,1,src.channels());
        const unsigned char *ptrs = src.ptr();
        unsigned char *pR = res.data(), *pG = res.data(0,0,0,1), *pB = res.data(0,0,0,2);
        cimg_forXY(res,x,y) { *(pB++) = *(ptrs++); *(pG++) = *(ptrs++); *(pR++) = *(ptrs++); }
        return res;
      }
      return CImg<ucharT>(src.ptr(),src.channels(),src.cols,src.rows,1,true).get_permute_axes("yzcx");