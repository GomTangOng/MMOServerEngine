#include "stdafx.h"
#include "protocol.h"

#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda_runtime_api.h"
#include "device_functions.h"
#include "cuda_device_runtime_api.h"

#ifdef __INTELLISENSE__
void __syncthreads();
#endif
#ifndef __CUDACC__ 
//#define CUDA_CALLABLE_MEMBER 
#define __CUDACC__
//#define CUDA_CALLABLE_MEMBER __host__ __device__
#endif

#define imin(a, b) (a<b ? a : b)


#define N               10000
#define ThreadsPerBlock 256
#define BlocksPerGrid   imin(32, (N+ThreadsPerBlock -1)/ThreadsPerBlock)

static void HandleError(cudaError_t err,
	const char *file,
	int line) {
	if (err != cudaSuccess) {
		printf("%s in %s at line %d\n", cudaGetErrorString(err),
			file, line);
		exit(EXIT_FAILURE);
	}
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))

struct CUDA_OBJECT
{	
	float x, y, z;
	float radian;
	float radius;

	/*__device__ bool SphereCollsion(CUDA_OBJECT *obj)
	{
		float dx = (obj->x - x) * (obj->x - x);
		float dy = (obj->y - y) * (obj->y - y);
		float dz = (obj->z - z) * (obj->z - z);
		float sr = (obj->radius + radius) * (obj->radius + radius);
		if (dx + dy + dz < sr) return true;
		return false;
	}*/
};
