    XmpValue& XmpValue::operator=(const XmpValue& rhs)
    {
        if (this == &rhs) return *this;
        xmpArrayType_ = rhs.xmpArrayType_;
        xmpStruct_ = rhs.xmpStruct_;
        return *this;
    }