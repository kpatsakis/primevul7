bool ConstantFolding::IsReductionSimplifiableToIdentity(
    const NodeDef& node, const TensorShapeProto& input_shape, bool keep_dims,
    const TensorVector& reduction_indices_vector) const {
  int output_size = reduction_indices_vector[0]->NumElements();
  if (output_size == 0) {
    return true;
  }

  if (!keep_dims) {
    return false;
  }
  bool simplifiable = true;
  for (int i = 0; i < output_size; ++i) {
    int64_t dim;
    if (reduction_indices_vector[0]->dtype() == DT_INT32) {
      dim = reduction_indices_vector[0]->flat<int32>()(i);
    } else {
      dim = reduction_indices_vector[0]->flat<int64_t>()(i);
    }
    if (dim < 0) {
      dim += input_shape.dim_size();
    }
    if (dim < 0 || dim >= input_shape.dim_size() ||
        input_shape.dim(dim).size() != 1) {
      simplifiable = false;
      break;
    }
  }
  return simplifiable;
}