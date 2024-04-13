    DataValue::DataValue(const byte* buf,
              long len, ByteOrder byteOrder,TypeId typeId)
        : Value(typeId)
    {
        read(buf, len, byteOrder);
    }