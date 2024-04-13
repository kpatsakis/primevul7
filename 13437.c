_cairo_image_traps_compositor_get (void)
{
    static cairo_atomic_once_t once = CAIRO_ATOMIC_ONCE_INIT;
    static cairo_traps_compositor_t compositor;

    if (_cairo_atomic_init_once_enter(&once)) {
	_cairo_traps_compositor_init(&compositor,
				     &__cairo_no_compositor);
	compositor.acquire = acquire;
	compositor.release = release;
	compositor.set_clip_region = set_clip_region;
	compositor.pattern_to_surface = _cairo_image_source_create_for_pattern;
	compositor.draw_image_boxes = draw_image_boxes;
	//compositor.copy_boxes = copy_boxes;
	compositor.fill_boxes = fill_boxes;
	compositor.check_composite = check_composite;
	compositor.composite = composite;
	compositor.lerp = lerp;
	//compositor.check_composite_boxes = check_composite_boxes;
	compositor.composite_boxes = composite_boxes;
	//compositor.check_composite_traps = check_composite_traps;
	compositor.composite_traps = composite_traps;
	//compositor.check_composite_tristrip = check_composite_traps;
#if PIXMAN_VERSION >= PIXMAN_VERSION_ENCODE(0,22,0)
	compositor.composite_tristrip = composite_tristrip;
#endif
	compositor.check_composite_glyphs = check_composite_glyphs;
	compositor.composite_glyphs = composite_glyphs;

	_cairo_atomic_init_once_leave(&once);
    }

    return &compositor.base;
}