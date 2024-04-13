static inline unsigned int tok_check_len(size_t len)
{
    if (unlikely(len > MAX_TEXT))
	nasm_fatal("impossibly large token");

    return len;
}