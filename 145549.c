    std::ostream& XmpTextValue::write(std::ostream& os) const
    {
        bool del = false;
        if (xmpArrayType() != XmpValue::xaNone) {
            switch (xmpArrayType()) {
            case XmpValue::xaAlt: os << "type=\"Alt\""; break;
            case XmpValue::xaBag: os << "type=\"Bag\""; break;
            case XmpValue::xaSeq: os << "type=\"Seq\""; break;
            case XmpValue::xaNone: break; // just to suppress the warning
            }
            del = true;
        }
        else if (xmpStruct() != XmpValue::xsNone) {
            switch (xmpStruct()) {
            case XmpValue::xsStruct: os << "type=\"Struct\""; break;
            case XmpValue::xsNone: break; // just to suppress the warning
            }
            del = true;
        }
        if (del && !value_.empty()) os << " ";
        return os << value_;
    }