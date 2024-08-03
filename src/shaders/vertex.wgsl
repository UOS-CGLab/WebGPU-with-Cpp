@group(0) @binding(0) var<storage, read> vertex_V: array<i32>;
@group(0) @binding(1) var<storage, read> offset_V: array<i32>;
@group(0) @binding(2) var<storage, read> valance_V: array<i32>;
@group(0) @binding(3) var<storage, read> index_V: array<i32>;
@group(0) @binding(4) var<storage, read> pointIdx_V: array<i32>;
@group(0) @binding(5) var<storage, read_write> baseVertex: array<f32>;


@compute @workgroup_size(256)
fn compute_VertexPoint(@builtin(global_invocation_id) global_invocation_id: vec3<u32>){
    let id = global_invocation_id.x;
    let start = u32(vertex_V[0]*4);

    let index = vertex_V[id];
    let oldIndex = index_V[id];
    let offset = offset_V[id];
    let valance = valance_V[id]/2;

    var pos = vec3(0.0,0.0,0.0);

    for (var i=offset; i<offset+(valance*2) ; i++ ){
    pos.x = pos.x + ( baseVertex[pointIdx_V[i]*4]) / (f32(valance)*f32(valance));
    pos.y = pos.y + ( baseVertex[(pointIdx_V[i]*4)+1] ) / (f32(valance)*f32(valance));
    pos.z = pos.z + ( baseVertex[(pointIdx_V[i]*4)+2] ) / (f32(valance)*f32(valance));
    }

    baseVertex[start+id*4] = (pos.x + (baseVertex[oldIndex*4] * (f32(valance-2)/f32(valance))));
    baseVertex[start+id*4+1] = pos.y + (baseVertex[oldIndex*4+1] * (f32(valance-2)/f32(valance)));
    baseVertex[start+id*4+2] = pos.z + (baseVertex[oldIndex*4+2] * (f32(valance-2)/f32(valance)));
    baseVertex[start+id*4+3] = 0;
}