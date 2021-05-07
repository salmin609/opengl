#version 430 core

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout(std430, binding = 0) buffer positionBuffer
{
	vec3 positions[];
};

layout(std430, binding = 1) buffer velocityBuffer
{
	vec4 velocities[];
};

layout(std430, binding = 2) buffer attratorBuffer
{
	vec4 attractors[];
};

layout(std430, binding = 3) buffer lifeBuffer
{
	float lifes[];
};

uniform float dt;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float VecLen(vec3 v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3 normalize (vec3 v)
{
	return v / VecLen(v);
}

vec3 calcForceFor (vec3 forcePoint, vec3 pos)
{
	// Force:
	float gauss = 10000.0;
	float e = 2.71828183;
	float k_weak = 1.0;
	vec3 dir = forcePoint - pos.xyz;
	float g = pow (e, -pow(VecLen(dir), 2) / gauss);
	vec3 f = normalize(dir) * k_weak * (1+ mod(rand(dir.xy), 10) - mod(rand(dir.yz), 10)) / 10.0 * g;
	return f;
}

void main(void)
{
	uint index = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x;

	float newDt = dt * 100.0;
	vec3 forcePoint = vec3(0);

	for(int i = 0 ; i < 32; ++i)
	{
		forcePoint += attractors[i].xyz;
	}

	vec4 vel = velocities[index];
	vec3 pos = positions[index];
	float newW = lifes[index];

	float kv = 1.5;
	vec3 f = calcForceFor(forcePoint, pos) + rand(pos.xz) / 100.0;
	vec3 v = normalize(vel.xyz + (f * newDt)) * kv;

	v += (forcePoint - pos) * 0.00005;

	vec3 s = pos + v * newDt;
	newW -= 0.0001f * newDt;

	if(newW <= 0)
	{
		s = -s + rand(s.xy) * 20.0 - rand(s.yz) * 20.0;
		newW = 0.99f;
	}
	lifes[index] = newW;
	positions[index] = s;
	velocities[index] = vec4(v, vel.w);
}