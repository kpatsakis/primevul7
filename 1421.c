Status ConstantFolding::SimplifyNode(NodeDef* node, GraphDef* optimized_graph,
                                     GraphProperties* properties) {
  bool graph_modified_cached = graph_modified_;
  graph_modified_ = false;

  bool use_shape_info = properties->has_properties();
  RETURN_IF_MODIFIED(RemoveSplitOrSplitV(*properties, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(RemoveShuffleOrTranspose(
      *properties, use_shape_info, optimized_graph, node));
  RETURN_IF_MODIFIED(
      RemoveRandomShuffle(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(
      RemoveReverse(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(
      SimplifySlice(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(
      SimplifyStridedSlice(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(
      SimplifyTile(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_ERROR_OR_MODIFIED(
      SimplifyPad(*properties, use_shape_info, optimized_graph, node));
  RETURN_IF_MODIFIED(
      SimplifySqueeze(*properties, use_shape_info, optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(SimplifyPack(optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(MoveConstantsPastEnter(optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(SimplifySwitch(optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(
      SimplifyReduction(optimized_graph, *properties, node));
  SET_AND_RETURN_IF_MODIFIED(
      SimplifyReshape(*properties, use_shape_info, node));
  RETURN_IF_ERROR_OR_MODIFIED(SimplifyArithmeticOperations(
      *properties, use_shape_info, optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(ReduceDivToReciprocalMul(optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(
      ConstantPushDown(properties, optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(
      MulConvPushDown(optimized_graph, node, *properties));
  SET_AND_RETURN_IF_MODIFIED(PartialConstPropThroughIdentityN(node));
  SET_AND_RETURN_IF_MODIFIED(
      PartialAssocOpConstFolding(optimized_graph, properties, node));
  SET_AND_RETURN_IF_MODIFIED(
      MergeConcat(use_shape_info, properties, optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(
      PartialConcatConstFolding(optimized_graph, properties, node));
  SET_AND_RETURN_IF_MODIFIED(
      ConstantPushDownBiasAdd(properties, optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(SimplifyCase(optimized_graph, node));
  SET_AND_RETURN_IF_MODIFIED(
      SimplifySelect(*properties, optimized_graph, node));
  RETURN_IF_MODIFIED(
      RemoveRedundantVariableUpdates(properties, optimized_graph, node));

  graph_modified_ = graph_modified_cached;
  return Status::OK();
}