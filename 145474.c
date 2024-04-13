    uint64_t Image::byteSwap(uint64_t value,bool bSwap) const
    {
        uint64_t result = 0;
        byte* source_value = reinterpret_cast<byte *>(&value);
        byte* destination_value = reinterpret_cast<byte *>(&result);

        for (int i = 0; i < 8; i++)
            destination_value[i] = source_value[8 - i - 1];

        return bSwap ? result : value;
    }