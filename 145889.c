    std::istream& operator>>(std::istream& is, URational& r)
    {
        // http://dev.exiv2.org/boards/3/topics/1912?r=1915
        /// \todo This implementation seems to be duplicated for the Rational type. Try to remove duplication
        if ( std::tolower(is.peek()) == 'f' ) {
            char  F = 0;
            float f = 0.f;
            is >> F >> f ;
            f  = 2.0f * std::log(f) / std::log(2.0f) ;
            r  = Exiv2::floatToRationalCast(f);
        } else {
            uint32_t nominator = 0;
            uint32_t denominator = 0;
            char c('\0');
            is >> nominator >> c >> denominator;
            if (c != '/')
                is.setstate(std::ios::failbit);
            if (is)
                r = std::make_pair(nominator, denominator);
        }
        return is;
    }