struct Uniforms {
    matrix: mat4x4f,
    view: vec3f,
    time: f32,
    wireAdjust: f32,
};

@group(0) @binding(0) var<uniform> uni: Uniforms;
@group(0) @binding(1) var<storage, read_write> baseVertex: array<f32>;

@compute @workgroup_size(256) fn cs(@builtin(global_invocation_id) global_invocation_id: vec3<u32>){
    let id = global_invocation_id.x;
    _ = uni.time;

    baseVertex[id*4+0] = baseVertex[id*4+0];
    // baseVertex[id*4+1] = sin(uni.time*f32(id)/10)+cos(uni.time*f32(id)/10) + (baseVertex[id*4+0] + baseVertex[id*4+2])/2;
    baseVertex[id*4+1] = baseVertex[id*4+1];
    baseVertex[id*4+2] = baseVertex[id*4+2];
    baseVertex[id*4+3] = 0;
}