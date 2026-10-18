import SwiftUI
import MetalKit
import UIKit

@main
struct LegacyMobileApp: App {
    var body: some Scene {
        WindowGroup {
            RendererScreen()
                .preferredColorScheme(.dark)
        }
    }
}

struct RendererScreen: View {
    @State private var engineStatus = LegacyEngineBridge.runCoreSmokeTest()

    var body: some View {
        ZStack(alignment: .topLeading) {
            Color(red: 0.035, green: 0.055, blue: 0.09)
                .ignoresSafeArea()
            MetalView()
                .ignoresSafeArea()
            VStack(alignment: .leading, spacing: 6) {
                Text("LegacyMobile")
                    .font(.system(size: 27, weight: .bold, design: .rounded))
                Text("iOS · Metal renderer")
                    .font(.system(size: 14, weight: .medium, design: .monospaced))
                    .foregroundStyle(.white.opacity(0.72))
                Text(engineStatus)
                    .font(.system(size: 11, weight: .medium, design: .monospaced))
                    .foregroundStyle(.green.opacity(0.95))
                    .padding(.top, 4)
            }
            .padding(.horizontal, 22)
            .padding(.top, 18)
            VStack {
                Spacer()
                Text("C++ Vec3 source linked · full game engine integration still in progress")
                    .font(.system(size: 11, weight: .medium))
                    .foregroundStyle(.white.opacity(0.68))
                    .multilineTextAlignment(.center)
                    .padding(.horizontal, 24)
                    .padding(.bottom, 24)
            }
            .frame(maxWidth: .infinity)
        }
    }
}

struct MetalView: UIViewRepresentable {
    func makeUIView(context: Context) -> MTKView {
        let view = MTKView(frame: .zero, device: MTLCreateSystemDefaultDevice())
        view.colorPixelFormat = .bgra8Unorm
        view.clearColor = MTLClearColor(red: 0.035, green: 0.055, blue: 0.09, alpha: 1)
        view.preferredFramesPerSecond = 60
        view.enableSetNeedsDisplay = false
        view.isPaused = false
        view.delegate = context.coordinator
        context.coordinator.configure(view)
        return view
    }

    func updateUIView(_ uiView: MTKView, context: Context) {}

    func makeCoordinator() -> MetalRenderer {
        MetalRenderer()
    }
}
