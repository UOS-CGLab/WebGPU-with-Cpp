struct Uniforms {
    matrix: mat4x4f,
    view: vec3f,
    time: f32,
    wireAdjust: f32,
};

struct Vertex {
    @location(0) position : vec4f,
};

struct VSOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
};

@group(0) @binding(0) var<uniform> uni: Uniforms;

@vertex fn vs(
    vert: Vertex,
) -> VSOutput {
    var vsOut: VSOutput;

    let p = vert.position.xyz;

    // vsOut.position = uni.matrix * vec4f(p*250, 1);
    vsOut.position = uni.matrix * vec4f(p*5, 1);
    return vsOut;
}

@fragment fn fs(vsOut: VSOutput) -> @location(0) vec4f {
    return vec4f(0.5, 0, 0, 1);
}