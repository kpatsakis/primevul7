bool IsValidConstShapeForMulConvPushDown(
    const string& data_format, const TensorShapeProto& filter_shape,
    const TensorShapeProto& mul_const_input_shape) {
  // If the const is a scalar, or it has fewer or same number of dimensions
  // than the filter and it only has single element, the optimization should
  // work.
  if (mul_const_input_shape.dim_size() <=
          static_cast<int>(data_format.size()) &&
      TensorShape(mul_const_input_shape).num_elements() == 1) {
    return true;
  }

  // Otherwise, check the eligibility according to data format.
  if (data_format == "NHWC" || data_format == "NDHWC") {
    TensorShapeProto new_filter_shape;
    if (!ShapeAfterBroadcast(filter_shape, mul_const_input_shape,
                             &new_filter_shape)) {
      return false;
    }
    if (!ShapesSymbolicallyEqual(filter_shape, new_filter_shape)) {
      return false;
    }
    // Only the last dimension could be larger than one, since broadcasting over
    // the last dimension (the output channel) will result in invalid filter.
    for (int i = 0; i < mul_const_input_shape.dim_size() - 1; ++i) {
      if (mul_const_input_shape.dim(i).size() > 1) return false;
    }
    return true;
  } else if (data_format == "NCHW" || data_format == "NCDHW") {
    // TODO(laigd): support NCHW and NCDHW (b/111214513).
    return false;
  }
  return false;
}