#include <metal_stdlib>
using namespace metal;

struct VoxelVertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VoxelVertexOut vertex_main(uint vertexID [[vertex_id]],
                                  const device float4 *vertices [[buffer(0)]]) {
    VoxelVertexOut out;
    out.position = vertices[vertexID * 2];
    out.color = vertices[vertexID * 2 + 1];
    return out;
}

fragment float4 fragment_main(VoxelVertexOut in [[stage_in]]) {
    return in.color;
}
