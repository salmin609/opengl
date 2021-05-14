#include "FluidCompute.h"
#include "FluidParticle.h"

FluidCompute::FluidCompute()
{
	particles = new WaterParticle*[pTotalNum];
	grid = new FluidGrid(wxMin, wxMax, wyMin, wyMax, wzMin, wzMax, h, h);
	
	for(int i = 0; i < pxNum; ++i)
	{
		for(int j = 0 ; j < pyNum; ++j)
		{
			for(int k = 0; k < pzNum; ++k)
			{
				WaterParticle* tempParticle = new WaterParticle();
				tempParticle->pos.x = pStartX + static_cast<float>(i) * pDist;
				tempParticle->pos.y = pStartY + static_cast<float>(j) * pDist;
				tempParticle->pos.z = pStartZ + static_cast<float>(k) * pDist;

				tempParticle->force.x = 0.f;
				tempParticle->force.y = -gravity;
				tempParticle->force.z = 0.f;

				tempParticle->vel.x = 0.f;
				tempParticle->vel.y = 0.f;
				tempParticle->vel.z = 0.f;

				tempParticle->density = 0.f;
				tempParticle->lambda = 0.f;

				int index = i * pyNum * pzNum + j * pzNum + k;
				tempParticle->id = index;

				particles[index] = tempParticle;
				
				Vector3 pos{tempParticle->pos.x, tempParticle->pos.y, tempParticle->pos.z};
				particlesPos.push_back(pos);
				grid->Add(tempParticle);
			}
		}
	}
	
}

FluidCompute::~FluidCompute()
{
	for (int i = 0; i < pTotalNum; i++) {
		delete[] particles[i];
	}
	delete[] particles;
	particlesPos.clear();
	delete grid;
}

void FluidCompute::Update()
{
	PredictPosition();

	for(int i = 0 ; i < 3; ++i)
	{

		
		CollisionDetection();
	}

	
	UpdateVelocityPos();

	grid->Adjust();
}

void FluidCompute::PredictPosition()
{
	for(int i = 0; i < threadNum; ++i)
	{
		threads.push_back(std::thread([&, i]()
		{
			for(int j = i * workLoad; j < (i + 1) * workLoad && j < pTotalNum; ++j)
			{
				pWaterParticle particle = particles[j];

				particle->vel.x += particle->force.x * tStep;
				particle->vel.y += particle->force.y * tStep;
				particle->vel.z += particle->force.z * tStep;

				particle->force.x = 0;
				particle->force.y = -gravity;
				particle->force.z = 0;

				particle->pos.x = particlesPos[j].x + particle->vel.x * tStep;
				particle->pos.y = particlesPos[j].y + particle->vel.y * tStep;
				particle->pos.z = particlesPos[j].z + particle->vel.z * tStep;
			}
		}));
	}

	for(int i = 0; i < threadNum; ++i)
	{
		threads[i].join();
	}
	threads.clear();
}

void FluidCompute::UpdateVelocityPos()
{
	for(int i = 0; i < threadNum; ++i)
	{
		threads.push_back(std::thread([&, i]()
		{
			for(int j = i * workLoad; j < (i + 1) * workLoad && j < pTotalNum; ++j)
			{
				pWaterParticle particle = particles[j];
				Vector3& predictedPos = particlesPos[j];

				particle->vel.x = (particle->pos.x - predictedPos.x) / tStep;
				particle->vel.y = (particle->pos.y - predictedPos.y) / tStep;
				particle->vel.z = (particle->pos.z - predictedPos.z) / tStep;

				std::vector<Indices> neighbors;
				neighbors.reserve(100);
				grid->GetNeighbors(particle, neighbors);
				
				for(std::vector<Indices>::iterator it = neighbors.begin(); it != neighbors.end(); ++it)
				{
					int ix = it->ix;
					int iy = it->iy;
					int iz = it->iz;

					std::set<pWaterParticle>* g = grid->Get(ix, iy, iz);

					for(std::set<pWaterParticle>::iterator p = g->begin(); p != g->end(); ++p)
					{
						int k = (*p)->id;
						pWaterParticle neighborParticle = particles[k];

						float w = CalculateW(j, k);
						float vijx = neighborParticle->vel.x - particle->vel.x;
						float vijy = neighborParticle->vel.y - particle->vel.y;
						float vijz = neighborParticle->vel.z - particle->vel.z;
						float c = 0.001f;
						particle->vel.x += c * vijx * w;
						particle->vel.y += c * vijy * w;
						particle->vel.z += c * vijz * w;
					}
				}
				predictedPos.x = particle->pos.x;
				predictedPos.y = particle->pos.y;
				predictedPos.z = particle->pos.z;
			}
		}));
	}

	for(int i = 0 ; i < threadNum; ++i)
	{
		threads[i].join();
	}
	threads.clear();
}

void FluidCompute::CollisionDetection()
{
	for(int i = 0 ; i < threadNum; ++i)
	{
		threads.push_back(std::thread([&, i]()
		{
			for(int j = i * workLoad; j < (i + 1) * workLoad && j < pTotalNum; ++j)
			{
				pWaterParticle particle = particles[j];

				if(particle->pos.x < wxMin)
				{
					particle->pos.x = wxMin;
					particle->vel.x = -particle->vel.x;
				}
				if (particle->pos.x > wxMax)
				{
					particle->pos.x = wxMax;
					particle->vel.x = -particle->vel.x;
				}
				
				if (particle->pos.y < wyMin)
				{
					particle->pos.y = wyMin;
					particle->vel.y = -particle->vel.y;
				}
				if (particle->pos.y > wyMax)
				{
					particle->pos.y = wyMax;
					particle->vel.y = -particle->vel.y;
				}

				if (particle->pos.z < wzMin)
				{
					particle->pos.z = wzMin;
					particle->vel.z = -particle->vel.z;
				}
				if (particle->pos.z > wzMax)
				{
					particle->pos.z = wzMax;
					particle->vel.z = -particle->vel.z;
				}
			}
		}));
	}
	for(int i = 0; i < threadNum; ++i)
	{
		threads[i].join();
	}
	threads.clear();
}

int FluidCompute::ParticleTotalNum()
{
	return pTotalNum;
}

float FluidCompute::CalculateW(int i, int j)
{
	pWaterParticle jParticle = particles[j];
	pWaterParticle iParticle = particles[i];

	float dx = jParticle->pos.x - iParticle->pos.x;
	float dy = jParticle->pos.y - iParticle->pos.y;
	float dz = jParticle->pos.z - iParticle->pos.z;

	float rsqr = static_cast<float>(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

	if (hsqr < rsqr)
		return 0.0f;

	return wk * static_cast<float>(pow(hsqr - rsqr, 3));
}

float* FluidCompute::ParticlePositionData()
{
	return &particlesPos[0].x;
}

std::vector<Vector3> FluidCompute::ParticlePos()
{
	return particlesPos;
}
