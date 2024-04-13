bool ConstantFolding::IsReductionCandidateForSimplification(
    const NodeDef& node, const GraphProperties& properties,
    TensorShapeProto* input_tensor_shape, TensorShapeProto* output_tensor_shape,
    bool* is_single_element_op) const {
  // Get the properties of the input & output tensors and check if they both
  // contain a single element.
  if (!properties.HasInputProperties(node.name()) ||
      !properties.HasOutputProperties(node.name())) {
    return false;
  }
  const auto& input_props = properties.GetInputProperties(node.name())[0];
  const auto& output_props = properties.GetOutputProperties(node.name())[0];
  if (!input_props.has_shape() || input_props.shape().unknown_rank() ||
      !output_props.has_shape() || output_props.shape().unknown_rank()) {
    return false;
  }
  *input_tensor_shape = input_props.shape();
  *output_tensor_shape = output_props.shape();
  for (int i = 0; i < input_tensor_shape->dim_size(); ++i) {
    if (input_tensor_shape->dim(i).size() < 0) {
      return false;
    }
  }
  for (int i = 0; i < output_tensor_shape->dim_size(); ++i) {
    if (output_tensor_shape->dim(i).size() < 0) {
      return false;
    }
  }
  const int input_num_elements =
      TensorShape(*input_tensor_shape).num_elements();
  const int output_num_elements =
      TensorShape(*output_tensor_shape).num_elements();
  *is_single_element_op = input_num_elements == 1 && output_num_elements == 1;

  return true;
}