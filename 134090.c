static void delete_Blocks(void)
{
    Token *block, *blocktmp;

    list_for_each_safe(block, blocktmp, tokenblocks)
        nasm_free(block);

    freeTokens = tokenblocks = NULL;
}