static void alloc_file_image(MemBuffer &mb, off_t size)
{
    assert(mem_size_valid_bytes(size));
    if (mb.getVoidPtr() == NULL) {
        mb.alloc(size);
    } else {
        assert(size <= (off_t) mb.getSize());
    }
}