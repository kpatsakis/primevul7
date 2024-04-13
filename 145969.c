    Value::UniquePtr Value::create(TypeId typeId)
    {
        UniquePtr value;
        switch (typeId) {
        case invalidTypeId:
        case signedByte:
        case unsignedByte:
            value = UniquePtr(new DataValue(typeId));
            break;
        case asciiString:
            value = UniquePtr(new AsciiValue);
            break;
        case unsignedShort:
            value = UniquePtr(new ValueType<uint16_t>);
            break;
        case unsignedLong:
        case tiffIfd:
            value = UniquePtr(new ValueType<uint32_t>(typeId));
            break;
        case unsignedRational:
            value = UniquePtr(new ValueType<URational>);
            break;
        case undefined:
            value = UniquePtr(new DataValue);
            break;
        case signedShort:
            value = UniquePtr(new ValueType<int16_t>);
            break;
        case signedLong:
            value = UniquePtr(new ValueType<int32_t>);
            break;
        case signedRational:
            value = UniquePtr(new ValueType<Rational>);
            break;
        case tiffFloat:
            value = UniquePtr(new ValueType<float>);
            break;
        case tiffDouble:
            value = UniquePtr(new ValueType<double>);
            break;
        case string:
            value = UniquePtr(new StringValue);
            break;
        case date:
            value = UniquePtr(new DateValue);
            break;
        case time:
            value = UniquePtr(new TimeValue);
            break;
        case comment:
            value = UniquePtr(new CommentValue);
            break;
        case xmpText:
            value = UniquePtr(new XmpTextValue);
            break;
        case xmpBag:
        case xmpSeq:
        case xmpAlt:
            value = UniquePtr(new XmpArrayValue(typeId));
            break;
        case langAlt:
            value = UniquePtr(new LangAltValue);
            break;
        default:
            value = UniquePtr(new DataValue(typeId));
            break;
        }
        return value;
    } // Value::create