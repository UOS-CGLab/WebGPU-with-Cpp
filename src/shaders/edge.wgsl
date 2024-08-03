@group(0) @binding(0) var<storage, read> vertex_E: array<i32>;
@group(0) @binding(1) var<storage, read> pointIdx_E: array<i32>;
@group(0) @binding(2) var<storage, read_write> baseVertex: array<f32>;

@compute @workgroup_size(256)
fn compute_EdgePoint(@builtin(global_invocation_id) global_invocation_id: vec3<u32>){
    let id = global_invocation_id.x;
    let start2 = u32(vertex_E[0]*4);

    let index = vertex_E[id];
    let offset = 4*id;

    var pos = vec3(0.0,0.0,0.0);

    for (var i=offset; i<offset+4 ; i++ ){
    pos.x = pos.x + ( baseVertex[pointIdx_E[i]*4]) / 4;
    pos.y = pos.y + ( baseVertex[(pointIdx_E[i]*4)+1] ) / 4;
    pos.z = pos.z + ( baseVertex[(pointIdx_E[i]*4)+2] ) / 4;
    }

    baseVertex[start2+id*4] = pos.x;
    baseVertex[start2+id*4+1] = pos.y;
    baseVertex[start2+id*4+2] =pos.z;
    baseVertex[start2+id*4+3] = 0;
}