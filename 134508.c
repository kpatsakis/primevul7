static int megasas_next_index(MegasasState *s, int index, int limit)
{
    index++;
    if (index == limit) {
        index = 0;
    }
    return index;
}