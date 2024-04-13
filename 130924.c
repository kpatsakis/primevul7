__debug_dir_hdr(struct zzip_dir_hdr *hdr)
{
    if (sizeof(struct zzip_dir_hdr) > sizeof(struct zzip_disk_entry))
        { WARN1("internal sizeof-mismatch may break wreakage"); }
    /*  the internal directory structure is never bigger than the
     *  external zip central directory space had been beforehand
     *  (as long as the following assertion holds...)
     */

    if (((zzip_off_t) hdr) & 3)
    {
        NOTE1("this machine's malloc(3) returns sth. not u32-aligned");
    }
    /* we assume that if this machine's malloc has returned a non-aligned
     * memory block, then it is actually safe to access misaligned data, and
     * since it does only affect the first hdr it should not even bring about
     * too much of that cpu's speed penalty
     */
}