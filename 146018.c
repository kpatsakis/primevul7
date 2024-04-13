    std::ostream& CommentValue::write(std::ostream& os) const
    {
        CharsetId csId = charsetId();
        if (csId != undefined) {
            os << "charset=\"" << CharsetInfo::name(csId) << "\" ";
        }
        return os << comment();
    }