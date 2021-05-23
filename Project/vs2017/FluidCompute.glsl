#version 430
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;

const int neighborCount = 300;
const int pTotalCount = 20 * 20 * 20;

struct ParticleNeighbors
{
	int neighborIndices[neighborCount];
};

struct ParticleVec4
{
	vec4 velocity;
	vec4 force;
	vec4 predictedPos;
};
struct ParticleVal
{
	float density;
	float lambda;
	int id;
};
struct BubbleVec4
{
	vec4 bubblePos;
	vec4 bubbleVel;
};

layout(std430, binding = 0) buffer particleVec4
{
	ParticleVec4 particleInfoVec4[];
};


layout(std430, binding = 1) buffer particleFloat
{
	ParticleVal particleInfoValue[];
};

layout(std430, binding = 2) buffer neighbors
{
	ParticleNeighbors neighbor[];
};

layout(std430, binding = 3) buffer bubbleTypes
{
	int bubbleType[];
};

layout(std430, binding = 4) buffer bubbleRadiuses
{
	float bubbleRadius[];
};

layout(std430, binding = 5) buffer bubbleVec4s
{
	BubbleVec4 bubbleVec4Val[];
};

layout(std430, binding = 6) buffer bubbleLifetimes
{
	float bubbleLifetime[];
};

layout(std430, binding = 7) buffer particlePoses
{
	vec4 particlePos[];
};


const float PI = 3.1415926f;

const float tStep = 1.0f / 30;
const float gravity = 9.8f;
const float h = 0.14f;
const float hsqr = h * h;
const float sigma = 0.1f;
const float wk = 315.f / (64 * PI * float(pow(h, 9)));
const float dwk = 15.f / (PI * float(pow(h, 6)));

const float scorrk = wk * float(pow(0.99 * hsqr, 3));

const float pDensity0 = 1500.f;
const float pRadius = 20.f;
const float pMass = 1.25e-5f;

const float d = 0.5f;
const float wxMin = -d;
const float wxMax = 2.f * d;
const float wyMin = -2.f * d;
const float wyMax = d;
const float wzMin = -d;
const float wzMax = d;

const int xNum = int(ceil((wxMax - wxMin)/ h));
const int yNum = int(ceil((wyMax - wyMin) / h));
const int zNum = int(ceil((wzMax - wzMin) / h));

const int BubbleTypeNone = -1;
const int BubbleTypeSpray = 0;
const int BubbleTypeBubble = 1;
const int BubbleTypeFoam = 2;

const float minta=10;
const float maxta=45;
const float mink=0.1;
const float maxk=0.3;
const float kb=0.8;
const float kd=0.1;
const float mass=0.2;
const int kta=50;
const float maxlifetime=3;
const int LIMIT1 = 5;
const int LIMIT2 = 50;

float Clamp(float v, float minVal, float maxVal)
{
	if(v > maxVal)
		v = maxVal;

	if(v < minVal)
		v = minVal;

	return (v - minVal) / (maxVal - minVal);
}

float CalculateW(int i, int j)
{
	vec4 posI = particlePos[i];
	vec4 posJ = particlePos[j];

	float dx = posJ.x - posI.x;
	float dy = posJ.y - posI.y;
	float dz = posJ.z - posI.z;

	float rsqr = float(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

	if(hsqr < rsqr)
		return 0.0f;

	return wk * float(pow(hsqr - rsqr, 3));
}

ivec3 Normalize(ivec3 val)
{
	ivec3 result = ivec3(0);
	int x = val.x;
	int y = val.y;
	int z = val.z;

	if(x < 0)
		x = 0;
	if(x > xNum)
	

	return result;
}
vec3 GetVelocity(float x, float y, float z)
{
	vec3 result = vec3(0);

	if(x < wxMin || x > wxMax || y < wyMin || y > wyMax || z < wzMin || z > wzMax)
		return result;

	int ix = int((x - wxMin) / h);
	int iy = int((y - wyMin) / h);
	int iz = int((z - wzMin) / h);

	int ixMin = int((x - h - wxMin) / h);
	int ixMax = int((x + h - wxMin) / h);
	int iyMin = int((y - h - wyMin) / h);
	int iyMax = int((y + h - wyMin) / h);
	int izMin = int((z - h - wzMin) / h);
	int izMax = int((z + h - wzMin) / h);
	ivec3 minVal = ivec3(ixMin, iyMin, izMin);
	ivec3 maxVal = ivec3(ixMax, iyMax, izMax);

	ivec3 normalizeMinval = Normalize(minVal);
	ivec3 normalizeMaxval = Normalize(maxVal);

	int newIxMin = normalizeMinval.x;
	int newIxMax = normalizeMaxval.x;

	int newIyMin = normalizeMinval.y;
	int newIyMax = normalizeMaxval.y;

	int newIzMin = normalizeMinval.z;
	int newIzMax = normalizeMaxval.z;

	float d = 0.0;
	for(int i = newIxMin; i < newIxMax + 1; ++i)
	{
		for(int j = newIyMin; j < newIyMax + 1; ++j)
		{
			for(int k = newIzMin; k < newIzMax + 1; ++k)
			{
				int index = i * newIxMax * newIyMax + j * newIyMax + k;
				//int index = i * 20 + j * 20 * 20 + k;
				vec4 posVal = particlePos[index];
				vec4 velVal = particleInfoVec4[index].velocity;

				float dx = posVal.x - x;
				float dy = posVal.y - y;
				float dz = posVal.z - z;

				float rsqr = pow(dx, 2) + pow(dy, 2) + pow(dz, 2);

				if(hsqr > rsqr)
				{
					float tmp = wk * pow(hsqr - rsqr, 3);
					result = result + vec3(velVal.x * tmp, velVal.y * tmp, velVal.z * tmp);
					d = d + wk * pow(hsqr - rsqr, 3);
				}
			}
		}
	}
	if(d == 0.0f)
		return vec3(0.0f);

	result /= d;

	return result;
}

int GetDensity(float x, float y, float z)
{
	int result = 0;

	if(x < wxMin || x > wxMax || y < wyMin || y > wyMax || z < wzMin || z > wzMax)
		return result;

	int ix = int((x - wxMin) / h);
	int iy = int((y - wyMin) / h);
	int iz = int((z - wzMin) / h);

	int ixMin = int((x - h - wxMin) / h);
	int ixMax = int((x + h - wxMin) / h);
	int iyMin = int((y - h - wyMin) / h);
	int iyMax = int((y + h - wyMin) / h);
	int izMin = int((z - h - wzMin) / h);
	int izMax = int((z + h - wzMin) / h);

	ivec3 minVal = ivec3(ixMin, iyMin, izMin);
	ivec3 maxVal = ivec3(ixMax, iyMax, izMax);

	ivec3 normalizeMinval = Normalize(minVal);
	ivec3 normalizeMaxval = Normalize(maxVal);

	int newIxMin = normalizeMinval.x;
	int newIxMax = normalizeMaxval.x;

	int newIyMin = normalizeMinval.y;
	int newIyMax = normalizeMaxval.y;

	int newIzMin = normalizeMinval.z;
	int newIzMax = normalizeMaxval.z;

	for(int i = newIxMin; i < newIxMax + 1; ++i)
	{
		for(int j = newIyMin; j < newIyMax + 1; ++j)
		{
			for(int k = newIzMin; k < newIzMax + 1; ++k)
			{
				//int index = i * 20 + j * 20 * 20 + k;
				int index = i * newIxMax * newIyMax + j * newIyMax + k;
				vec4 posVal = particlePos[index];

				float dx = posVal.x - x;
				float dy = posVal.y - y;
				float dz = posVal.z - z;

				float r = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

				if(h > r)
					result++;
			}
		}
	}
	return result;
}

vec3 GetNormal(int index)
{
	vec3 result = vec3(0.0);
	vec4 posVal = particlePos[index];

	int d = GetDensity(posVal.x, posVal.y, posVal.z);
	int xPlus = GetDensity(posVal.x + h, posVal.y, posVal.z);
	int yPlus = GetDensity(posVal.x, posVal.y + h, posVal.z);
	int zPlus = GetDensity(posVal.x, posVal.y, posVal.z + h);

	vec3 temp = vec3(xPlus - d, yPlus - d, zPlus - d);
	result = normalize(temp);
	return result;
}

vec3 CalculateDW(int i, int j)
{
	vec3 result = vec3(0.0);

	if(i == j)
		return result;

	vec4 posI = particlePos[i];
	vec4 posJ = particlePos[j];

	float dx = posJ.x - posI.x;
	float dy = posJ.y - posI.y;
	float dz = posJ.z - posI.z;

	float r = float(sqrt(float(pow(dx, 2) + pow(dy, 2) + pow(dz, 2))));

	if(h < r)
		return result;

	float c = 3 * dwk * float(pow(h - r, 2) / (r + 1e-10));

	result.x = dx * c;
	result.y = dy * c;
	result.z = dz * c;

	return result;
}
float GetC(int i)
{
	//float densityVal = density[i];
	float densityVal = particleInfoValue[i].density;
	return densityVal / pDensity0 - 1;
}


void PredictPosition(uint index)
{
	//vec4 posVal = pos[index];
	vec4 posVal = particlePos[index];
	vec4 forceVal = particleInfoVec4[index].force;
	vec4 velVal =  particleInfoVec4[index].velocity;
	vec4 predictedPosVal =  particleInfoVec4[index].predictedPos;

	velVal.x += forceVal.x * tStep;
	velVal.y += forceVal.y * tStep;
	velVal.z += forceVal.z * tStep;

	forceVal.x = 0.f;
	forceVal.y = -gravity;
	forceVal.z = 0.f;

	posVal.x = predictedPosVal.x + velVal.x * tStep;
	posVal.y = predictedPosVal.y + velVal.y * tStep;
	posVal.z = predictedPosVal.z + velVal.z * tStep;

	particlePos[index]= posVal;
	particleInfoVec4[index].force = forceVal;
}
void ComputeDensity()
{
	int index = int(gl_GlobalInvocationID.x);
	float densityVal = particleInfoValue[index].density;

	int neighborVal[neighborCount] = neighbor[index].neighborIndices;
	uint neighborIndex = 0;

	while(neighborVal[neighborIndex] != -1)
	{
		if(neighborIndex > neighborCount - 1)
		{
			break;
		}
		
		int neighboringIndex = neighborVal[neighborIndex];
		float w = CalculateW(index, neighboringIndex);
		
		densityVal += pMass * w;

		neighborIndex++;
	}
	particleInfoValue[index].density = densityVal;
}

void UpdateVelocityPos()
{
	int index = int(gl_GlobalInvocationID.x);
	vec4 posVal = particlePos[index];
	//vec4 velVal = velocity[index];
	vec4 predictedPosVal = particleInfoVec4[index].predictedPos;

	particleInfoVec4[index].velocity.x = (posVal.x - predictedPosVal.x) / tStep;
	particleInfoVec4[index].velocity.y = (posVal.y - predictedPosVal.y) / tStep;
	particleInfoVec4[index].velocity.z = (posVal.z - predictedPosVal.z) / tStep;

	int neighborVal[neighborCount] = neighbor[index].neighborIndices;
	uint neighborIndex = 0;

	while(neighborVal[neighborIndex] != -1)
	{
		if(neighborIndex > neighborCount - 1)
		{
			break;
		}
		
		int neighboringIndex = neighborVal[neighborIndex];

		float w = CalculateW(index, neighboringIndex);
		
		vec4 neighborVel = particleInfoVec4[neighboringIndex].velocity;
		
		float vijx = neighborVel.x - particleInfoVec4[index].velocity.x;
		float vijy = neighborVel.y - particleInfoVec4[index].velocity.y;
		float vijz = neighborVel.z - particleInfoVec4[index].velocity.z;
		float c = 0.001f;

		particleInfoVec4[index].velocity.x += c * vijx * w;
		particleInfoVec4[index].velocity.y += c * vijy * w;
		particleInfoVec4[index].velocity.z += c * vijz * w;

		neighborIndex++;
	}
	predictedPosVal.x = posVal.x;
	predictedPosVal.y = posVal.y;
	predictedPosVal.z = posVal.z;

	//velocity[index] = velVal;
	particleInfoVec4[index].predictedPos = predictedPosVal;
}
void CollisionDetection()
{
	uint index = int(gl_GlobalInvocationID.x);
	vec4 posVal = particlePos[index];
	vec4 velVal = particleInfoVec4[index].velocity;

	if(posVal.x < wxMin)
	{
		posVal.x = wxMin;
		velVal.x *= -1;
	}
	if(posVal.x > wxMax)
	{
		posVal.x = wxMax;
		velVal.x *= -1;
	}

	if(posVal.y < wyMin)
	{
		posVal.y = wyMin;
		velVal.y *= -1;
	}
	if(posVal.y > wyMax)
	{
		posVal.y = wyMax;
		velVal.y *= -1;
	}

	if(posVal.z < wzMin)
	{
		posVal.z = wzMin;
		velVal.z *= -1;
	}
	if(posVal.z > wzMax)
	{
		posVal.z = wzMax;
		velVal.z *= -1;
	}

	particlePos[index] = posVal;
	particleInfoVec4[index].velocity = velVal;
}

void ComputeLambda()
{
	int index = int(gl_GlobalInvocationID.x);
	float lambdaVal = particleInfoValue[index].lambda;
	int neighborVal[neighborCount] = neighbor[index].neighborIndices;
	uint neighborIndex = 0;
	float gradCSqr = 0;
	vec3 sum = vec3(0.0);

	while(neighborVal[neighborIndex] != -1)
	{
		if(neighborIndex > neighborCount - 1)
		{
			break;
		}
		
		int neighboringIndex = neighborVal[neighborIndex];

		vec3 t = CalculateDW(index, neighboringIndex);
		gradCSqr += float(pow(t.x, 2) + pow(t.y, 2) + pow(t.z, 2));
		sum.x += t.x;
		sum.y += t.y;
		sum.z += t.z;

		neighborIndex++;
	}
	gradCSqr += float(pow(sum.x, 2) + pow(sum.y, 2) + pow(sum.z, 2));
	gradCSqr *= float(pow(pMass / pDensity0, 2));
	lambdaVal = -GetC(index) / (gradCSqr + sigma);

	particleInfoValue[index].lambda = lambdaVal;
}
void ComputeDeltaP()
{
	int index = int(gl_GlobalInvocationID.x);
	float lambdaVal = particleInfoValue[index].lambda;
	int neighborVal[neighborCount] = neighbor[index].neighborIndices;
	uint neighborIndex = 0;
	vec4 posVal = particlePos[index];
	vec3 sum = vec3(0.0);

	while(neighborVal[neighborIndex] != -1)
	{
		if(neighborIndex > neighborCount - 1)
		{
			break;
		}
		
		int neighboringIndex = neighborVal[neighborIndex];
		float neighborLambdaVal = particleInfoValue[neighboringIndex].lambda;

		float scorr = -0.1f * float(pow(CalculateW(index, neighboringIndex) / scorrk, 4));
		vec3 t = CalculateDW(neighboringIndex, index);
		float c = lambdaVal + neighborLambdaVal + scorr;

		t.x *= c;
		t.y *= c;
		t.z *= c;
		sum.x += t.x;
		sum.y += t.y;
		sum.z += t.z;

		neighborIndex++;
	}

	float t = pMass / pDensity0;
	sum.x *= t;
	sum.y *= t;
	sum.z *= t;
	posVal.x += sum.x;
	posVal.y += sum.y;
	posVal.z += sum.z;

	particlePos[index] = posVal;
}

void GenerateBubble()
{
	uint index = gl_GlobalInvocationID.x;
	vec4 posVal = particlePos[index];
	vec4 velVal = particleInfoVec4[index].velocity;
	float vel = float(sqrt(pow(velVal.x , 2) + pow(velVal.y , 2) + pow(velVal.z , 2)));

	if(vel == 0.0f)
		return;

	float delta = vel * tStep;
	float ita = 0.0f;
	vec3 ni = GetNormal(int(index));

	int neighborVal[neighborCount] = neighbor[index].neighborIndices;
	uint neighborIndex = 0;

	while(neighborVal[neighborIndex] != -1)
	{
		if(neighborIndex > neighborCount - 1)
		{
			break;
		}
		
		int neighboringIndex = neighborVal[neighborIndex];

		vec4 neighborPos = particlePos[neighboringIndex];
		vec4 neighborVel = particleInfoVec4[neighboringIndex].velocity;
		float dx = posVal.x - neighborPos.x;
		float dy = posVal.y - neighborPos.y;
		float dz = posVal.z - neighborPos.z;
		float r = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

		if(r == 0)
		{
			neighborIndex++;
			continue;
		}
		if(h < r)
		{
			neighborIndex++;
			continue;
		}
		float dvx = velVal.x - neighborVel.x;
		float dvy = velVal.y - neighborVel.y;
		float dvz = velVal.z - neighborVel.z;
		float dv = sqrt(pow(dvx, 2) + pow(dvy, 2) + pow(dvz, 2));
		ita+=dv*(1-(dx/r*dvx/dv+dy/r*dvy/dv+dz/r*dvz/dv))*(1-r/h);

		neighborIndex++;
	}

	ita = Clamp(ita, minta, maxta);

	float ik= 0.5 * mass * (pow(velVal.x ,2)+ pow(velVal.y ,2)+ pow(velVal.z ,2));

	ik = Clamp(ik, mink, maxk);

	int nd = int(ik * kta * ita * tStep) + 3;

	for(int i = 0; i < nd; ++i)
	{
		int bubbleIndex = 0;
		float posX = ((posVal.x - delta) + (posVal.x + tStep)) / 2;
		float posY = ((posVal.y - delta) + (posVal.y + tStep)) / 2;
		float posZ = ((posVal.z - delta) + (posVal.z + tStep)) / 2;
		float velX = velVal.x / 2;
		float velY = velVal.y / 2;
		float velZ = velVal.z / 2;

		float radius = 0.1f;
		vec4 pos = vec4(posX, posY, posZ, 1);
		vec4 vel = vec4(velX, velY, velZ, 1);
		float lifeTime = ik * maxlifetime;
		int type = BubbleTypeBubble;

		while(bubbleType[bubbleIndex] != BubbleTypeNone)
		{
			if(bubbleIndex > pTotalCount)
				break;

			bubbleIndex++;
		}

		bubbleVec4Val[bubbleIndex].bubblePos = pos;
		bubbleRadius[bubbleIndex] = radius;
		bubbleVec4Val[bubbleIndex].bubbleVel = vel;
		bubbleLifetime[bubbleIndex] = lifeTime;
		bubbleType[bubbleIndex] = type;
	}
}
void UpdateBubbles()
{
	uint index = gl_GlobalInvocationID.x;

	BubbleVec4 bubbleVal = bubbleVec4Val[index];
	float bubbleLifeVal = bubbleLifetime[index];
	int bubbleTypeVal = bubbleType[index];

	vec4 bubbleVelVal = bubbleVal.bubbleVel;
	vec4 bubblePosVal = bubbleVal.bubblePos;

	if(bubbleTypeVal == BubbleTypeNone)
	{
		return;
	}

	if(bubbleLifeVal < 0)
	{
		bubbleType[index] = BubbleTypeNone;
		return;
	}

	if(bubbleTypeVal == BubbleTypeSpray)
	{
		bubbleVelVal.y += -gravity * tStep;
		bubblePosVal.x += bubbleVelVal.x * tStep;
		bubblePosVal.y += bubbleVelVal.y * tStep;
		bubblePosVal.z += bubbleVelVal.z * tStep;
	}
	else if(bubbleTypeVal == BubbleTypeFoam)
	{
		vec3 v = GetVelocity(bubblePosVal.x, bubblePosVal.y, bubblePosVal.z);
		bubblePosVal.x += v.x * tStep;
		bubblePosVal.y += v.y * tStep;
		bubblePosVal.z += v.z * tStep;
	}
	else if(bubbleTypeVal == BubbleTypeBubble)
	{
		vec3 v = GetVelocity(bubblePosVal.x, bubblePosVal.y, bubblePosVal.z);
		bubbleVelVal.x += kd * (v.x - bubbleVelVal.x);
		bubbleVelVal.y += kb * gravity * tStep * kd * (v.y - bubbleVelVal.y);
		bubbleVelVal.z += kd * (v.z - bubbleVelVal.z);

		bubblePosVal.x += bubbleVelVal.x * tStep;
		bubblePosVal.y += bubbleVelVal.y * tStep;
		bubblePosVal.z += bubbleVelVal.z * tStep;
	}

	if(bubblePosVal.x < wxMin)
	{
		bubblePosVal.x = wxMin;
		bubbleVelVal.x *= -1;
	}
	else if(bubblePosVal.x > wxMax)
	{
		bubblePosVal.x = wxMax;
		bubbleVelVal.x *= -1;
	}

	if(bubblePosVal.y < wyMin)
	{
		bubblePosVal.y = wyMin;
		bubbleVelVal.y *= -1;
	}
	else if(bubblePosVal.y > wyMax)
	{
		bubblePosVal.y = wyMax;
		bubbleVelVal.y *= -1;
	}

	if(bubblePosVal.z < wzMin)
	{
		bubblePosVal.z = wzMin;
		bubbleVelVal.z *= -1;
	}
	else if(bubblePosVal.z > wzMax)
	{
		bubblePosVal.z = wzMax;
		bubbleVelVal.z *= -1;
	}

	int density = GetDensity(bubblePosVal.x, bubblePosVal.y, bubblePosVal.z);

	if(density < LIMIT1)
	{
		bubbleType[index] = BubbleTypeSpray;
	}
	else if(density < LIMIT2)
	{
		bubbleType[index] = BubbleTypeFoam;
	}

	bubbleLifetime[index] -= tStep;

	bubbleVec4Val[index].bubblePos = bubblePosVal;
	bubbleVec4Val[index].bubbleVel = bubbleVelVal;

}



void main()
{
	uint gid = gl_GlobalInvocationID.x;
	
	PredictPosition(gid);
	ComputeDensity();

	for(int i = 0 ; i < 3; ++i)
	{
		ComputeLambda();
		ComputeDeltaP();
		CollisionDetection();
	}
	
	UpdateVelocityPos();
	GenerateBubble();
	UpdateBubbles();
}