set_gc_signal(i_ctx_t *i_ctx_p, int value)
{
    gs_memory_gc_status_t stat;
    int i;

    for (i = 0; i < countof(idmemory->spaces_indexed); i++) {
        gs_ref_memory_t *mem = idmemory->spaces_indexed[i];
        gs_ref_memory_t *mem_stable;

        if (mem == 0)
            continue;
        for (;; mem = mem_stable) {
            mem_stable = (gs_ref_memory_t *)
                gs_memory_stable((gs_memory_t *)mem);
            gs_memory_gc_status(mem, &stat);
            stat.signal_value = value;
            gs_memory_set_gc_status(mem, &stat);
            if (mem_stable == mem)
                break;
        }
    }
}