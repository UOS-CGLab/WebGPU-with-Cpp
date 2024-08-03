struct Uniforms {
    matrix: mat4x4f,
    view: vec3f,
    time: f32,
    wireAdjust: f32,
};

struct IndexVertex {
    @location(0) position : vec2f,
};

struct Vertex {
    position : vec4f,
};

struct Connectivity {
    index: i32,
};

struct Color {
    value: vec4f,
    zFighting: vec3f,
};

struct VSOutput {
    @builtin(position) position: vec4f,
    @location(0) color: vec4f,
    @location(1) normal: vec3f,
    @location(2) center: vec3f,
    @location(3) adjust: f32,
};

@group(0) @binding(0) var<uniform> uni: Uniforms;
@group(0) @binding(1) var<storage, read> pos2: array<Vertex>;
@group(0) @binding(2) var<storage, read> texture: array<Vertex>;
// @group(0) @binding(2) var u_earthbump1k: texture_2d<f32>;
// @group(0) @binding(3) var sampler0: sampler;
@group(1) @binding(0) var<storage, read> conn: array<Connectivity>;
@group(1) @binding(1) var<storage, read> color: Color;

fn B0(t: f32) -> f32 {
    return (1.0/6.0)*(1.0-t)*(1.0-t)*(1.0-t);
}
fn B0prime(t: f32) -> f32 {
    return (-1.0/2.0)*(1.0-t)*(1.0-t);
}
fn B1(t: f32) -> f32 {
    return (1.0/6.0)*(3.0*t*t*t - 6.0*t*t + 4.0);
}
fn B1prime(t: f32) -> f32 {
    return (1.0/6.0)*(9.0*t*t - 12.0*t);
}
fn B2(t: f32) -> f32 {
    return (1.0/6.0)*(-3.0*t*t*t + 3.0*t*t + 3.0*t + 1.0);
}
fn B2prime(t: f32) -> f32 {
    return (1.0/6.0)*(-9.0*t*t + 6.0*t + 3.0);
}
fn B3(t: f32) -> f32 {
    return (1.0/6.0)*t*t*t;
}
fn B3prime(t: f32) -> f32 {
    return (1.0/2.0)*t*t;
}

fn length(t: vec3f) -> f32 {
    return sqrt(pow(t.x, 2)+pow(t.y, 2)+pow(t.z, 2));
}

@vertex fn vs(
    @builtin(instance_index) instanceIndex: u32,
    @builtin(vertex_index) vertexIndex: u32,
    vert: IndexVertex
) -> VSOutput {
    var vsOut: VSOutput;

    let p =  B0(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 0].index  ].position.xyz
            +B0(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 1].index  ].position.xyz
            +B0(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+ 2].index  ].position.xyz
            +B0(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+ 3].index  ].position.xyz
            +B1(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 4].index  ].position.xyz
            +B1(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 5].index  ].position.xyz
            +B1(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+ 6].index  ].position.xyz
            +B1(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+ 7].index  ].position.xyz
            +B2(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 8].index  ].position.xyz
            +B2(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 9].index  ].position.xyz
            +B2(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+10].index  ].position.xyz
            +B2(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+11].index  ].position.xyz
            +B3(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+12].index  ].position.xyz
            +B3(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+13].index  ].position.xyz
            +B3(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+14].index  ].position.xyz
            +B3(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+15].index  ].position.xyz;

    let tu = B0prime(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 0].index  ].position
            +B0prime(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 1].index  ].position
            +B0prime(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+ 2].index  ].position
            +B0prime(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+ 3].index  ].position
            +B1prime(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 4].index  ].position
            +B1prime(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 5].index  ].position
            +B1prime(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+ 6].index  ].position
            +B1prime(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+ 7].index  ].position
            +B2prime(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+ 8].index  ].position
            +B2prime(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+ 9].index  ].position
            +B2prime(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+10].index  ].position
            +B2prime(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+11].index  ].position
            +B3prime(vert.position.x)*B0(vert.position.y)*pos2[  conn[instanceIndex*16+12].index  ].position
            +B3prime(vert.position.x)*B1(vert.position.y)*pos2[  conn[instanceIndex*16+13].index  ].position
            +B3prime(vert.position.x)*B2(vert.position.y)*pos2[  conn[instanceIndex*16+14].index  ].position
            +B3prime(vert.position.x)*B3(vert.position.y)*pos2[  conn[instanceIndex*16+15].index  ].position;

    let tv = B0(vert.position.x)*B0prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 0].index  ].position
            +B0(vert.position.x)*B1prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 1].index  ].position
            +B0(vert.position.x)*B2prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 2].index  ].position
            +B0(vert.position.x)*B3prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 3].index  ].position
            +B1(vert.position.x)*B0prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 4].index  ].position
            +B1(vert.position.x)*B1prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 5].index  ].position
            +B1(vert.position.x)*B2prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 6].index  ].position
            +B1(vert.position.x)*B3prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 7].index  ].position
            +B2(vert.position.x)*B0prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 8].index  ].position
            +B2(vert.position.x)*B1prime(vert.position.y)*pos2[  conn[instanceIndex*16+ 9].index  ].position
            +B2(vert.position.x)*B2prime(vert.position.y)*pos2[  conn[instanceIndex*16+10].index  ].position
            +B2(vert.position.x)*B3prime(vert.position.y)*pos2[  conn[instanceIndex*16+11].index  ].position
            +B3(vert.position.x)*B0prime(vert.position.y)*pos2[  conn[instanceIndex*16+12].index  ].position
            +B3(vert.position.x)*B1prime(vert.position.y)*pos2[  conn[instanceIndex*16+13].index  ].position
            +B3(vert.position.x)*B2prime(vert.position.y)*pos2[  conn[instanceIndex*16+14].index  ].position
            +B3(vert.position.x)*B3prime(vert.position.y)*pos2[  conn[instanceIndex*16+15].index  ].position;

    let imageWidth: f32 = 512;  // 예시 이미지 너비
    let imageHeight: f32 = 512;  // 예시 이미지 높이

    // 텍스처 좌표에 따라 이미지에서 변위 값을 샘플링
    let texCoord = vert.position; // Vertex 구조체에 texCoords가 있다고 가정
    let xIndex = i32(texCoord.x * imageWidth);
    let yIndex = i32(texCoord.y * imageHeight);
    let index = xIndex + yIndex * i32(imageWidth);
    let displacement = texture[index];  // 이미지 데이터에서 변위 값 조회

    // vsOut.position = uni.matrix * vec4f(p*5, 1-displacement.position.x*0.01);
    vsOut.position = uni.matrix * vec4f(p*5, 1);

    vsOut.center = vec3f(vert.position.xy, 0);

    let g0 = pos2[  conn[instanceIndex*16+ 5].index  ].position.xyz;
    let g1 = pos2[  conn[instanceIndex*16+ 6].index  ].position.xyz;
    let g2 = pos2[  conn[instanceIndex*16+ 9].index  ].position.xyz;
    let g3 = pos2[  conn[instanceIndex*16+10].index  ].position.xyz;

    // let uv = vec2<f32>(0.5, 0.5);
    // let temp = textureSample(u_earthbump1k, sampler0, uv);

    let normal = normalize(  cross(  (uni.matrix*tu).xyz, (uni.matrix*tv).xyz  )  );
    vsOut.normal = normal;
    let view = normalize(  -1*uni.view - p  );
    _ = color;
    // vsOut.color = vec4f( p, 1);
    vsOut.color = color.value;
    vsOut.color += color.value * vec4f(dot(view, normal)*0.5, dot(view, normal)*0.5, dot(view, normal)*0.5, 1)  ;
    if(dot(view, normal) > 0)
    {
        vsOut.color += vec4f((pow(dot(view, normal), 100)*0.2), (pow(dot(view, normal), 100)*0.2), (pow(dot(view, normal), 100)*0.2), 1);
    }

    var wire = uni.wireAdjust;
    if(uni.wireAdjust == 1)
    {
        vsOut.color = vec4f(0, 0, 0, 0);
        wire = 1;
    }
    vsOut.adjust = wire/(length(g1 - g0)+length(g3-g2)+length(g2 - g0)+length(g3-g1));
    return vsOut;
}

@fragment fn fs(vsOut: VSOutput) -> @location(0) vec4f {
    // return vsOut.color;

    // if(vsOut.normal.z < 0.0)
    // {
    //     discard;
    // }
    if(abs(vsOut.center.x - 0.5) > (0.5-vsOut.adjust) || abs(vsOut.center.y - 0.5) > (0.5-vsOut.adjust)) // 0.49 vsOut.adjust
    {
        return vec4f(0, 0, 0, 1);
    }
    return vsOut.color;
    // return vec4f(vsOut.center, 1);
}