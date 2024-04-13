static inline bool xdp_map_invalid(const struct bpf_prog *xdp_prog,
				   unsigned long aux)
{
	return (unsigned long)xdp_prog->aux != aux;
}