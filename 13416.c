_cairo_image_spans_compositor_get (void)
{
    static cairo_atomic_once_t once = CAIRO_ATOMIC_ONCE_INIT;
    static cairo_spans_compositor_t spans;
    static cairo_compositor_t shape;

    if (_cairo_atomic_init_once_enter(&once)) {
	_cairo_shape_mask_compositor_init (&shape,
					   _cairo_image_traps_compositor_get());
	shape.glyphs = NULL;

	_cairo_spans_compositor_init (&spans, &shape);

	spans.flags = 0;
#if PIXMAN_HAS_OP_LERP
	spans.flags |= CAIRO_SPANS_COMPOSITOR_HAS_LERP;
#endif

	//spans.acquire = acquire;
	//spans.release = release;
	spans.fill_boxes = fill_boxes;
	spans.draw_image_boxes = draw_image_boxes;
	//spans.copy_boxes = copy_boxes;
	spans.pattern_to_surface = _cairo_image_source_create_for_pattern;
	//spans.check_composite_boxes = check_composite_boxes;
	spans.composite_boxes = composite_boxes;
	//spans.check_span_renderer = check_span_renderer;
	spans.renderer_init = span_renderer_init;
	spans.renderer_fini = span_renderer_fini;

	_cairo_atomic_init_once_leave(&once);
    }

    return &spans.base;
}