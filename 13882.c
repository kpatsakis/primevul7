static const char *get_index_name(const EnumNames names[], uint32_t index)
{
    while (names->name != NULL && names->id != index) {
        names++;
    }

    return names->name;
}