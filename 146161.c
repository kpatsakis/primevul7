    Rational parseRational(const std::string& s, bool& ok)
    {
        Rational ret = stringTo<Rational>(s, ok);
        if (ok) return ret;

        long l = stringTo<long>(s, ok);
        if (ok) return Rational(l, 1);

        float f = stringTo<float>(s, ok);
        if (ok) return floatToRationalCast(f);

        bool b = stringTo<bool>(s, ok);
        if (ok) return b ? Rational(1, 1) : Rational(0, 1);

        // everything failed, return from stringTo<Rational> is probably the best fit
        return ret;
    }