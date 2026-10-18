# iOS port

The XcodeGen project definition lives in `project.yml`; GitHub Actions generates the Xcode project on its macOS runner and archives an unsigned IPA.

The current native app establishes the first actual iOS/Metal rendering target and displays a voxel-shaped test scene. This is a renderer foundation, not a claim that the legacy C++ game engine is already integrated or playable. The existing engine still needs an Apple platform layer, a source-available renderer backend, and startup/resource integration.

An unsigned IPA is not directly installable on a physical iPhone by itself. It must be signed with an appropriate development/distribution certificate and provisioning profile or through a compatible sideloading workflow.
