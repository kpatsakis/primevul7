Status ConstantFolding::EvaluateNode(const NodeDef& node,
                                     const TensorVector& inputs,
                                     TensorVector* output) const {
  return ::tensorflow::grappler::EvaluateNode(node, inputs, cpu_device_,
                                              resource_mgr_.get(), output);
}