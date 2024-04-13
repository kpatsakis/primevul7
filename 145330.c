    long parseLong(const std::string& s, bool& ok)
    {
        long ret = stringTo<long>(s, ok);
        if (ok) return ret;

        float f = stringTo<float>(s, ok);
        if (ok) return static_cast<long>(f);

        Rational r = stringTo<Rational>(s, ok);
        if (ok) {
            if (r.second == 0) {
                ok = false;
                return 0;
            }
            return static_cast<long>(static_cast<float>(r.first) / r.second);
        }

        bool b = stringTo<bool>(s, ok);
        if (ok) return b ? 1 : 0;

        // everything failed, return from stringTo<long> is probably the best fit
        return ret;
    }