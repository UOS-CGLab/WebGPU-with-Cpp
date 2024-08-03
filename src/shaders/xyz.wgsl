struct OurVertexShaderOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
};

struct Uniforms {
    matrix: mat4x4f,
};

@group(0) @binding(0) var<uniform> uni: Uniforms;

@vertex fn vs(
    @builtin(vertex_index) vertexIndex : u32
) -> OurVertexShaderOutput {
    let pos = array(
    // x축
    vec3f( -1.0,  0.0,  0.0 ),
    vec3f(  1.0,  0.0,  0.0 ),
    // y축
    vec3f(  0.0, -1.0,  0.0 ),
    vec3f(  0.0,  1.0,  0.0 ),
    // z축
    vec3f(  0.0,  0.0, -1.0 ),
    vec3f(  0.0,  0.0,  1.0 ),
    );
    let color = array(
    vec4f(1, 0, 0, 1),
    vec4f(0, 1, 0, 1),
    vec4f(0, 0, 1, 1),
    );

    _ = uni;

    var vsOutput: OurVertexShaderOutput;
    let xyz = pos[vertexIndex];
    vsOutput.position = uni.matrix * vec4f(xyz*100, 1.0);
    vsOutput.color = color[vertexIndex/2];
    return vsOutput;
}

@fragment fn fs(fsInput: OurVertexShaderOutput) -> @location(0) vec4f {
    return fsInput.color;
}