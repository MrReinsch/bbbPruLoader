//============================================================================
// Name        : bbbPruLoader.cpp
// Author      : René Reinsch
// Version     :
// Copyright   : Your copyright notice
// Description : BeagleBone Black PRU Asm loader in C++
//============================================================================

#include <iostream>
#include "prussdrv.h"
#include <pruss_intc_mapping.h>
using namespace std;

#define PRU_NUM_0 	 0
#define PRU_NUM_1    1

static int initPruSystem(void);
static int allocatePruMem(void);
static int closePruSystem(void);

/******************************************************************************
* Global Variable Definitions                                                 *
******************************************************************************/
void *pruSharedDataMem;
void *pru0DataMem;
void *pru1DataMem;
static unsigned int *pruSharedDataMem_uint;
static unsigned int *pru0DataMem_uint;
static unsigned int *pru1DataMem_uint;


int main(int argc, char* argv[]) {
	cout << "BeagleBone Black Pru Loader" << endl;
	int pruNo=-1;
	int file=-1;
	for(int i=0;i<argc;i++){ // pars param list
		if(string("-h").compare(argv[i])==0){
			cout << "BeagleBone Black Pru Loader - Help" << endl;
			cout << "-i <input filename> -> *.bin " << endl;
			cout << "-p <pru number> -> 0=PRU0, 1=PRU1" << endl;
			return 0;
		}
		if(string("-i").compare(argv[i])==0){
			if(argc > i){
				i++;
				file=i;
				continue;
			}
		}
		if(string("-p").compare(argv[i])==0){
			if(argc > i){
				i++;
				if(string("0").compare(argv[i])==0){
					pruNo=PRU_NUM_0;
				}else if (string("1").compare(argv[i])==0){
					pruNo=PRU_NUM_1;
				}
				continue;
			}
		}
	}//
	if(pruNo<0){ // params
		cout << "Missing Pru Number!" << endl;
		cout << "-p <pru number> -> 0=PRU0, 1=PRU1" << endl;
		return -1;
	}
	if(file<0){
		cout << "Missing input file" << endl;
		cout << "-i <input filename> -> *.bin " << endl;
		return -1;
	}//
	int ret=initPruSystem(); // init PRU
	if(ret < 0){
		cout << "Init Pru failed" << endl;
		return -1;
	}
	//copy n run
	if(pruNo==PRU_NUM_0){
		ret=prussdrv_exec_program (PRU_NUM_0,argv[file]);
	}else{
		ret=prussdrv_exec_program (PRU_NUM_1,argv[file]);
	}
	if(ret < 0){
		cout << "Running ASM Pru failed" << endl;
		return -1;
	}
	// Wait to abort
	bool wait=true;
	cout << "Press key and enter to exit" << endl;
	char array[2];
	while(wait==true){
		cin.getline(array,2);
		wait=false;
	}
	closePruSystem();
	return 0;
}


/*****************************************************************************
*   static int initPruSystem(void)                                           *
*****************************************************************************/
static int initPruSystem(void){
	int ret=0;
	tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
	 /* Initialize the PRU */
     ret = prussdrv_init();
	/* Open PRU Interrupt */
	ret = prussdrv_open(PRU_EVTOUT_0);
	if (ret){
	    return ret;
	}
	/* Get the interrupt initialized */
	ret = prussdrv_pruintc_init(&pruss_intc_initdata);
	if (ret){
	    return ret;
	}
	allocatePruMem();
	return 0;
}

/*****************************************************************************
*   static int allocatePruMem(void)                                          *
*****************************************************************************/
static int allocatePruMem(void){
	int ret=0;
	/* Memory Allocation */
	/* Shared Mem */
	ret = prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &pruSharedDataMem);
	if(ret){
#ifdef DEBUG_PRINTF
	printf("prussdrv_map_prumem PRUSS0_SHARED_DATARAM failed\n");
#endif
		return ret;
	}
	pruSharedDataMem_uint = (unsigned int*) pruSharedDataMem;
	/*Pru 0 */
	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMem);
	if(ret){
#ifdef DEBUG_PRINTF
	printf("prussdrv_map_prumem PRUSS0_PRU0_DATARAM failed\n");
#endif
		return ret;
	}
	pru0DataMem_uint = (unsigned int*) pru0DataMem;
	/*Pru 1 */
	prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1DataMem);
	if(ret){
#ifdef DEBUG_PRINTF
	printf("prussdrv_map_prumem PRUSS0_PRU1_DATARAM failed\n");
#endif
		return ret;
	}
	pru1DataMem_uint = (unsigned int*) pru1DataMem;
	return 0;
}

/*****************************************************************************
*   static int closePruSystem(void)                                          *
*****************************************************************************/
static int closePruSystem(void){
	int ret=0;
	/* Disable PRU and close memory mapping*/
	ret = prussdrv_pru_disable(PRU_NUM_0);
	if(ret){
#ifdef DEBUG_PRINTF
		printf("prussdrv_pru_disable PRU0 failed\n");
#endif
		return ret;
	}
	ret = prussdrv_pru_disable(PRU_NUM_1);
	if(ret){
#ifdef DEBUG_PRINTF
		printf("prussdrv_pru_disable PRU0 failed\n");
#endif
		return ret;
	}
	prussdrv_exit();
#ifdef DEBUG_PRINTF
	printf("\n prussdrv_pru_disable pass\n");
#endif
	return ret;
}

int debugSend(void){


	    return 0;
}
