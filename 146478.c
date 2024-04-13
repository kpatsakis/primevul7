    float parseFloat(const std::string& s, bool& ok)
    {
        float ret = stringTo<float>(s, ok);
        if (ok) return ret;

        Rational r = stringTo<Rational>(s, ok);
        if (ok) {
            if (r.second == 0) {
                ok = false;
                return 0.0;
            }
            return static_cast<float>(r.first) / r.second;
        }

        bool b = stringTo<bool>(s, ok);
        if (ok) return b ? 1.0f : 0.0f;

        // everything failed, return from stringTo<float> is probably the best fit
        return ret;
    }