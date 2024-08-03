@group(0) @binding(0) var<storage, read> vertex_F: array<i32>;
@group(0) @binding(1) var<storage, read> offset_F: array<i32>;
@group(0) @binding(2) var<storage, read> valance_F: array<i32>;
@group(0) @binding(3) var<storage, read> pointIdx_F: array<i32>;
@group(0) @binding(4) var<storage, read_write> baseVertex: array<f32>;


@compute @workgroup_size(256)
fn compute_FacePoint(@builtin(global_invocation_id) global_invocation_id: vec3<u32>){
    let id = global_invocation_id.x;
    let start = u32(vertex_F[0]*4);

    let index = vertex_F[id];
    let offset = offset_F[id];
    let valance = valance_F[id];

    var pos = vec3(0.0,0.0,0.0);

    for (var i=offset; i<offset+valance ; i++ ){
    pos.x = pos.x + ( baseVertex[pointIdx_F[i]*4]) / f32(valance);
    pos.y = pos.y + ( baseVertex[(pointIdx_F[i]*4)+1] / f32(valance));
    pos.z = pos.z + ( baseVertex[(pointIdx_F[i]*4)+2] / f32(valance));
    }

    baseVertex[start+id*4] = pos.x;
    baseVertex[start+id*4+1] = pos.y;
    baseVertex[start+id*4+2] = pos.z;
    baseVertex[start+id*4+3] = 0;
}