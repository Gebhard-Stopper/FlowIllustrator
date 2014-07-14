#version 430 compatibility
#extension GL_ARB_compute_shader :  enable
#extension GL_ARB_shader_storage_buffer_object : enable;

layout( std140, binding=4) buffer col
{
	vec3 Colours[ ];
};

layout( std140, binding=5) buffer Vec
{
	vec2 Vectors[ ];
};

layout( local_size_x = 32, local_size_y = 1, local_size_z = 1 ) in;

uint gid = gl_GlobalInvocationID.x;

vec2 v = Vectors[ gid ].xy;
Colours