Status ConstantFolding::Optimize(Cluster* cluster, const GrapplerItem& item,
                                 GraphDef* optimized_graph) {
  // TensorFlow flushes denormals to zero and rounds to nearest, so we do
  // the same here.
  port::ScopedFlushDenormal flush;
  port::ScopedSetRound round(FE_TONEAREST);
  nodes_to_preserve_ = item.NodesToPreserve();
  for (const auto& feed : item.feed) {
    feed_nodes_.insert(NodeName(feed.first));
  }

  if (cpu_device_ == nullptr) {
    owned_device_.reset(new DeviceSimple());
    cpu_device_ = owned_device_.get();
  }

  graph_contains_assign_or_inplace_op_ = false;
  for (const NodeDef& node : item.graph.node()) {
    if (ModifiesInputsInPlace(node) || HasRefInput(node)) {
      graph_contains_assign_or_inplace_op_ = true;
      break;
    }
  }

  has_fetch_ = !item.fetch.empty();
  GrapplerItem item_to_optimize = item;
  GraphProperties properties(item_to_optimize);
  // It's possible to feed a placeholder with a tensor of any shape: make sure
  // that the shape inference deals with this conservatively unless we're in
  // aggressive mode.
  const bool assume_valid_feeds = opt_level_ == RewriterConfig::AGGRESSIVE;
  if (!properties
           .InferStatically(assume_valid_feeds,
                            /*aggressive_shape_inference=*/false,
                            /*include_input_tensor_values=*/false,
                            /*include_output_tensor_values=*/true)
           .ok()) {
    properties.Clear();
  }

  *optimized_graph = GraphDef();
  item_to_optimize.graph.Swap(optimized_graph);
  int64_t node_count;

  do {
    GRAPPLER_RETURN_IF_DEADLINE_EXCEEDED();
    graph_modified_ = false;
    item_to_optimize.graph.Swap(optimized_graph);
    node_count = item_to_optimize.graph.node_size();
    TF_RETURN_IF_ERROR(RunOptimizationPass(cluster, &item_to_optimize,
                                           &properties, optimized_graph));
  } while (graph_modified_ || optimized_graph->node_size() != node_count);
  *optimized_graph->mutable_library() = item.graph.library();
  *optimized_graph->mutable_versions() = item.graph.versions();

  return Status::OK();
}