
      static double mp_display_memory(_cimg_math_parser& mp) {
        cimg::unused(mp);
        std::fputc('\n',cimg::output());
        mp.mem.display("[" cimg_appname "_math_parser] Memory snapshot");
        return cimg::type<double>::nan();