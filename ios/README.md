# iOS port

The XcodeGen project definition lives in `project.yml`; GitHub Actions generates the Xcode project on its macOS runner and archives an unsigned IPA.

The native app now links the legacy C++ Vec3, Random, and AABB sources. Its startup smoke test exercises vector math, deterministic random values, and the engine's axis-aligned collision checks. The Metal scene is still a renderer test, not the playable game. The next blockers are portable world/chunk loading, the Apple platform layer, and a Metal backend for the legacy renderer contract.

An unsigned IPA is not directly installable on a physical iPhone by itself. It must be signed with an appropriate development/distribution certificate and provisioning profile or through a compatible sideloading workflow.
