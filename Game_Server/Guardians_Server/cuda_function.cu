#include "cuda_function.cuh"

struct TEST
{
	int a, b, c;

	__global__ void Test()
	{
		cout << "dddd\n" << endl;
	}
};

extern "C" void PrintPropertyDevice()
{
	cudaDeviceProp prop;

	int dev_cnt;
	
	HANDLE_ERROR(cudaGetDeviceCount(&dev_cnt));

	for (int i = 0; i < dev_cnt; ++i)
	{
		HANDLE_ERROR(cudaGetDeviceProperties(&prop, i));
		cout << " --- General Inforamtion for device " << i << " ---\n";
		cout << "Name : " << prop.name << endl;
		cout << "Compute capability : " << prop.major << "." << prop.minor << endl;
		cout << "Clock rate : " << prop.clockRate << endl;
		cout << "Device copy overlap : ";
		
		if (prop.deviceOverlap)
			cout << "Enabled\n";
		else
			cout << "Disabled\n";
		cout << "Kernel execution timeout : ";
		if (prop.kernelExecTimeoutEnabled)
			cout << "Enabled\n";
		else
			cout << "Disabled\n";

		cout << " --- Memory Information for device " << i << " --- \n";
		cout << "Total global mem : " << prop.totalGlobalMem << endl;
		cout << "Total constant mem : " << prop.totalConstMem << endl;
		cout << "Max mem pitch : " << prop.memPitch << endl;
		cout << "Texture Alignment : " << prop.textureAlignment << endl;
		cout << " --- MP Information for device " << i << " --- \n";
		cout << "Multiprocessor count : " << prop.multiProcessorCount << endl;
		cout << "Shared mem per block : " << prop.sharedMemPerBlock << endl;
		cout << "registers per block : " << prop.regsPerBlock << endl;
		cout << "Threads in warp : " << prop.warpSize << endl;
		cout << "Max threads per block " << prop.maxThreadsPerBlock << endl;
		cout << "Max thread dimensions : ( " << prop.maxThreadsDim[0] << " , "
											 << prop.maxThreadsDim[1] << " , " 
											 << prop.maxThreadsDim[2] << " )" << endl;
		cout << "Max grid dimensions : ( " << prop.maxGridSize[0] << " , "
										   << prop.maxGridSize[1] << " , "
										   << prop.maxGridSize[2] << " )" << endl;
	}
}

extern "C" void CudaInit()
{

}

//CUDA_OBJECT *dev_obj;
//int blocks;
//
//__global__ void SphereCollsion(CUDA_OBJECT *obj_list, int obj_cnt, int my_id, bool *result)
//{
//	int t_id = threadIdx.x + blockIdx.x * blockDim.x;
//	int stride = blockDim.x * gridDim.x;
//
//	while (t_id < obj_cnt)
//	{
//		if (obj_list[my_id].SphereCollsion(&obj_list[t_id]))
//		{
//			*result = true;
//		}
//		t_id += stride;
//	}
//}
//
//__global__ void AABBCollision()
//{
//
//}
//
//__global__ void OBBCollision()
//{
//
//}
//
//extern "C" void Init_Cuda()
//{
//	cudaDeviceProp prop;
//	cudaGetDeviceProperties(&prop, 0);
//	blocks = prop.multiProcessorCount;
//
//	cudaMalloc((void**)&dev_obj, sizeof(CUDA_OBJECT) * MAX_CLIENT);
//	//cudaMemcpy(dev_buffer, buffer, SIZE, cudaMemcpyHostToDevice);
//
//	//cudaMalloc((void**)&dev_histo, 256 * sizeof(long));
//	//cudaMemset(dev_histo, 0, 256 * sizeof(int));
//}
//
//extern "C" void Excute_Cuda(CUDA_OBJECT *obj, int obj_cnt, int my_id, bool *result)
//{
//	bool *dev_result;
//	cudaMalloc((void**)&dev_result, sizeof(bool));
//	cudaMemcpy(dev_obj, obj, sizeof(CUDA_OBJECT) * MAX_CLIENT, cudaMemcpyHostToDevice);
//
//	SphereCollsion << <blocks * 2, ThreadsPerBlock >> > (dev_obj, obj_cnt, my_id, dev_result);
//
//	cudaMemcpy(result, dev_result, sizeof(bool), cudaMemcpyDeviceToHost);
//
//	cudaFree(dev_result);
//}
//
//extern "C" void Release_Cuda()
//{
//	cudaFree(dev_obj);
//}

//extern "C" void Init_Cuda2(CUDA_OBJECT *obj_list, int obj_cnt, int my_id, bool *result)
//{
//	static int cnt = 0;
//	bool d_r;
//	CUDA_OBJECT *d_obj_list;
//
//	cudaMalloc((void**)&d_r, sizeof(bool));
//	cudaMemcpy((void**)&d_obj_list, &obj_list, sizeof(CUDA_OBJECT) * MAX_CLIENT, cudaMemcpyHostToDevice);
//
//	cudaDeviceProp prop;
//	cudaGetDeviceProperties(&prop, 0);
//	int blocks = prop.multiProcessorCount;
//
//	//SphereCollsion << <blocks * 2, 32 >> > (d_obj_list, obj_cnt, my_id, d_r);
//
//	cout << "Complete : " << ++cnt << endl;
//}