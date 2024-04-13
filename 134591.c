static inline uint32_t _clampbits(int x, uint32_t n) { 
	uint32_t _y_temp; 
	if( (_y_temp=x>>n) ) 
		x = ~_y_temp >> (32-n); 
	return x;
}