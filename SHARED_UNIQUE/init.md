#
## unique ptr

-   std::unique_ptr<VideoRenderer> local_renderer_;
-  VideoRenderer* local_renderer = local_renderer_.get();
-   local_renderer_.reset(new VideoRenderer(handle(), 1, 1, local_video));
-
```
namespace webrtc {

DefaultNetEqControllerFactory::DefaultNetEqControllerFactory() = default;
DefaultNetEqControllerFactory::~DefaultNetEqControllerFactory() = default;

std::unique_ptr<NetEqController>
DefaultNetEqControllerFactory::CreateNetEqController(
    const NetEqController::Config& config) const {
  return std::make_unique<DecisionLogic>(config);
}

}  // namespace webrtc
```
