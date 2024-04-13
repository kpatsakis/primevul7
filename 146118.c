    Exiv2::Exifdatum& setValue(Exiv2::Exifdatum& exifDatum, const T& value)
    {
        std::unique_ptr<Exiv2::ValueType<T> > v
            = std::unique_ptr<Exiv2::ValueType<T> >(new Exiv2::ValueType<T>);
        v->value_.push_back(value);
        exifDatum.value_ = std::move(v);
        return exifDatum;
    }