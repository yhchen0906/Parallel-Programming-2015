#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <fstream>  
#include <CL/cl.h>  

const char* programSource =   
    "__kernel                                                         \n"  
    "void hist(__global int *A,                                       \n"  
    "          __global int *C)                                       \n"  
    "{                                                                \n"  
    "    int idx = get_global_id(0);                                  \n"  
    "    atomic_inc(&C[(idx % 3) * 256 + A[idx]]);                    \n"  
    "}                                                                \n"  
    ;  

int main() {  
    unsigned int *A = NULL; 
    unsigned int *C = NULL; 
	std::ifstream inFile("input");
	std::ofstream outFile("0316236.out");
    unsigned int input_size;  
	inFile >> input_size;
    size_t datasize = sizeof(unsigned int) * input_size;  
    A = (unsigned int*)malloc(datasize);  
    C = (unsigned int*)malloc(sizeof(unsigned int) * 256 * 3);  
	memset(C, 0, sizeof(unsigned int) * 256 * 3);
    for (int i = 0; i < input_size; i++)
		inFile >> A[i];
    cl_int status;  
    cl_uint numPlatforms = 0;  
    status = clGetPlatformIDs(0, NULL, &numPlatforms);  
    cl_platform_id *platforms = NULL;  
    platforms=(cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));  
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);  
    cl_uint numDevices = 0;  
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);  
    cl_device_id *devices;  
    devices=(cl_device_id*)malloc(numDevices*sizeof(cl_device_id));  
    status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);  
    cl_context context;  
    context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);  
    cl_command_queue cmdQueue;  
    cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);  
    cl_mem bufA;  
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);  
    cl_mem bufC;  
    bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(unsigned int) * 256 * 3, NULL, &status);  
    status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL);  
    status = clEnqueueWriteBuffer(cmdQueue, bufC, CL_FALSE, 0, sizeof(unsigned int) * 256 * 3, C, 0, NULL, NULL);  
    cl_program program = clCreateProgramWithSource(context,   
        1,   
        (const char**)&programSource,   
        NULL,   
        &status);  
    status = clBuildProgram(program,        
        numDevices,                         
        devices,                            
        NULL,  
        NULL,  
        NULL  
        );  
    cl_kernel kernel;  
    kernel = clCreateKernel(program,  
        "hist",  
        &status);  
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);  
    status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufC);  
    size_t globalWorkSize[1];  
    globalWorkSize[0] = input_size;  
    status = clEnqueueNDRangeKernel(cmdQueue,       
        kernel,         
        1,              
        NULL,           
        globalWorkSize, 
        NULL,           
        0,              
        NULL,           
        NULL            
        );  
    clEnqueueReadBuffer(cmdQueue,     
        bufC,         
        CL_TRUE,      
        0,            
        sizeof(unsigned int) * 256 * 3,     
        C,            
        0,            
        NULL,         
        NULL          
        );  
	for (int i = 0; i < 256 * 3; i++) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << C[i] << ' ';
	}
    clReleaseKernel(kernel);  
    clReleaseProgram(program);  
    clReleaseCommandQueue(cmdQueue);  
    clReleaseMemObject(bufA);  
    clReleaseMemObject(bufC);  
    clReleaseContext(context);  
    free(A);  
    free(C);  
    free(platforms);  
    free(devices);  
    return 0;  
}
