static void free_tlist_array(Token **array, size_t nlists)
{
    Token **listp = array;

    while (nlists--)
        free_tlist(*listp++);

    nasm_free(array);
}