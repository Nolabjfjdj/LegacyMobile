import Metal
import MetalKit

private struct VoxelVertex {
    var position: SIMD4<Float>
    var color: SIMD4<Float>
}

final class MetalRenderer: NSObject, MTKViewDelegate {
    private var device: MTLDevice?
    private var commandQueue: MTLCommandQueue?
    private var pipeline: MTLRenderPipelineState?
    private var vertexBuffer: MTLBuffer?

    func configure(_ view: MTKView) {
        guard let device = view.device,
              let library = device.makeDefaultLibrary(),
              let vertexFunction = library.makeFunction(name: "vertex_main"),
              let fragmentFunction = library.makeFunction(name: "fragment_main"),
              let commandQueue = device.makeCommandQueue() else {
            return
        }
        self.device = device
        self.commandQueue = commandQueue

        let descriptor = MTLRenderPipelineDescriptor()
        descriptor.label = "LegacyMobile voxel renderer"
        descriptor.vertexFunction = vertexFunction
        descriptor.fragmentFunction = fragmentFunction
        descriptor.colorAttachments[0].pixelFormat = view.colorPixelFormat

        do {
            pipeline = try device.makeRenderPipelineState(descriptor: descriptor)
        } catch {
            pipeline = nil
            return
        }

        let vertices = Self.makeCubeVertices()
        vertexBuffer = device.makeBuffer(
            bytes: vertices,
            length: MemoryLayout<VoxelVertex>.stride * vertices.count,
            options: .storageModeShared
        )
    }

    func mtkView(_ view: MTKView, drawableSizeWillChange size: CGSize) {}

    func draw(in view: MTKView) {
        guard let pipeline,
              let vertexBuffer,
              let pass = view.currentRenderPassDescriptor,
              let drawable = view.currentDrawable,
              let commandBuffer = commandQueue?.makeCommandBuffer(),
              let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: pass) else {
            return
        }

        encoder.label = "Draw voxel test scene"
        encoder.setRenderPipelineState(pipeline)
        encoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 36)
        encoder.endEncoding()
        commandBuffer.present(drawable)
        commandBuffer.commit()
    }

    private static func makeCubeVertices() -> [VoxelVertex] {
        let top = SIMD4<Float>(0.42, 0.82, 0.36, 1)
        let left = SIMD4<Float>(0.22, 0.58, 0.28, 1)
        let right = SIMD4<Float>(0.15, 0.40, 0.22, 1)
        let front = SIMD4<Float>(0.30, 0.68, 0.31, 1)
        let back = SIMD4<Float>(0.18, 0.48, 0.25, 1)
        let bottom = SIMD4<Float>(0.12, 0.32, 0.18, 1)
        let p0 = SIMD4<Float>(-0.42, -0.42, 0.55, 1)
        let p1 = SIMD4<Float>( 0.42, -0.42, 0.55, 1)
        let p2 = SIMD4<Float>( 0.42,  0.42, 0.55, 1)
        let p3 = SIMD4<Float>(-0.42,  0.42, 0.55, 1)
        let p4 = SIMD4<Float>(-0.42, -0.42, -0.35, 1)
        let p5 = SIMD4<Float>( 0.42, -0.42, -0.35, 1)
        let p6 = SIMD4<Float>( 0.42,  0.42, -0.35, 1)
        let p7 = SIMD4<Float>(-0.42,  0.42, -0.35, 1)

        func face(_ a: SIMD4<Float>, _ b: SIMD4<Float>, _ c: SIMD4<Float>,
                  _ d: SIMD4<Float>, _ color: SIMD4<Float>) -> [VoxelVertex] {
            [
                VoxelVertex(position: a, color: color),
                VoxelVertex(position: b, color: color),
                VoxelVertex(position: c, color: color),
                VoxelVertex(position: a, color: color),
                VoxelVertex(position: c, color: color),
                VoxelVertex(position: d, color: color)
            ]
        }

        return face(p3, p2, p6, p7, top)
            + face(p0, p3, p7, p4, left)
            + face(p1, p5, p6, p2, right)
            + face(p0, p4, p5, p1, front)
            + face(p4, p7, p6, p5, back)
            + face(p0, p1, p2, p3, bottom)
    }
}
