int js_isprimitive(js_State *J, int idx) { return stackidx(J, idx)->type != JS_TOBJECT; }