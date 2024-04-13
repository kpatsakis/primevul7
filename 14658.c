get_u32_le(const void *vp)
{
	const u_char *p = (const u_char *)vp;
	u_int32_t v;

	v  = (u_int32_t)p[0];
	v |= (u_int32_t)p[1] << 8;
	v |= (u_int32_t)p[2] << 16;
	v |= (u_int32_t)p[3] << 24;

	return (v);
}