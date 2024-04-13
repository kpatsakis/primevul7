
      static double mp_image_print(_cimg_math_parser& mp) {
        const unsigned int ind = (unsigned int)cimg::mod((int)_mp_arg(2),mp.listout.width());
        cimg::mutex(6);
        CImg<T> &img = mp.listout[ind];
        CImg<charT> title(256);
        std::fputc('\n',cimg::output());
        cimg_snprintf(title,title._width,"[ Image #%u ]",ind);
        img.print(title);
        cimg::mutex(6,0);
        return cimg::type<double>::nan();