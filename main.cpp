/*******************************************************************************
* Copyright (c) 2015-2017
* School of Electrical, Computer and Energy Engineering, Arizona State University
* PI: Prof. Shimeng Yu
* All rights reserved.
*   
* This source code is part of NeuroSim - a device-circuit-algorithm framework to benchmark 
* neuro-inspired architectures with synaptic devices(e.g., SRAM and emerging non-volatile memory). 
* Copyright of the model is maintained by the developers, and the model is distributed under 
* the terms of the Creative Commons Attribution-NonCommercial 4.0 International Public License 
* http://creativecommons.org/licenses/by-nc/4.0/legalcode.
* The source code is free and you can redistribute and/or modify it
* by providing that the following conditions are met:
*   
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer. 
*   
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*   
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* Developer list: 
*   Pai-Yu Chen     Email: pchen72 at asu dot edu 
*                     
*   Xiaochen Peng   Email: xpeng15 at asu dot edu
********************************************************************************/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <random>
#include <vector>
#include "Cell.h"
#include "Array.h"
#include "formula.h"
#include "NeuroSim.h"
#include "Param.h"
#include "IO.h"
#include "Train.h"
#include "Test.h"
#include "Mapping.h"
#include "Definition.h"

using namespace std;

int main() {
	gen.seed(0);
	
	/* Load in MNIST data */
	ReadTrainingDataFromFile("FMNSIT train patch1.txt", "FMNSIT train label1.txt");
	ReadTestingDataFromFile("FMNSIT test patch1.txt", "FMNIST test label1.txt");

	/* Initialization of synaptic array from input to hidden layer */
	arrayIH->Initialization<IdealDevice>();
	//arrayIH->Initialization<RealDevice>();
	//arrayIH->Initialization<MeasuredDevice>();
	//arrayIH->Initialization<SRAM>(param->numWeightBit);
	//arrayIH->Initialization<DigitalNVM>(param->numWeightBit,true);

	/* Initialization of synaptic array from input to hidden layer */
	arrayHH->Initialization<IdealDevice>();
	//arrayHH->Initialization<RealDevice>();
	//arrayHH->Initialization<MeasuredDevice>();
	//arrayHH->Initialization<SRAM>(param->numWeightBit);
	//arrayHH->Initialization<DigitalNVM>(param->numWeightBit,true);

	/* Initialization of synaptic array from hidden to output layer */
	arrayHO->Initialization<IdealDevice>();
	//arrayHO->Initialization<RealDevice>();
	//arrayHO->Initialization<MeasuredDevice>();
	//arrayHO->Initialization<SRAM>(param->numWeightBit);
	//arrayHO->Initialization<DigitalNVM>(param->numWeightBit,true);


	/* Initialization of NeuroSim synaptic cores */
	param->relaxArrayCellWidth = 0;
	NeuroSimSubArrayInitialize(subArrayIH, arrayIH, inputParameterIH, techIH, cellIH);
	param->relaxArrayCellWidth = 0;//�ǹ� ? �𸣰���
	NeuroSimSubArrayInitialize(subArrayHH, arrayHH, inputParameterHH, techHH, cellHH);
	param->relaxArrayCellWidth = 1;
	NeuroSimSubArrayInitialize(subArrayHO, arrayHO, inputParameterHO, techHO, cellHO);
	/* Calculate synaptic core area */
	NeuroSimSubArrayArea(subArrayIH);
	NeuroSimSubArrayArea(subArrayHH);
	NeuroSimSubArrayArea(subArrayHO);
	
	/* Calculate synaptic core standby leakage power */
	NeuroSimSubArrayLeakagePower(subArrayIH);
	NeuroSimSubArrayLeakagePower(subArrayHH);
	NeuroSimSubArrayLeakagePower(subArrayHO);
	
	/* Initialize the neuron peripheries */
	NeuroSimNeuronInitialize(subArrayIH, inputParameterIH, techIH, cellIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH);
	NeuroSimNeuronInitialize(subArrayHH, inputParameterHH, techHH, cellHH, adderHH, muxHH, muxDecoderHH, dffHH, subtractorHH);
	NeuroSimNeuronInitialize(subArrayHO, inputParameterHO, techHO, cellHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayIH */
	double heightNeuronIH, widthNeuronIH;
	NeuroSimNeuronArea(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH, &heightNeuronIH, &widthNeuronIH);
	double leakageNeuronIH = NeuroSimNeuronLeakagePower(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayHH */
	double heightNeuronHH, widthNeuronHH;
	NeuroSimNeuronArea(subArrayHH, adderHH, muxHH, muxDecoderHH, dffHH, subtractorHH, &heightNeuronHH, &widthNeuronHH);
	double leakageNeuronHH = NeuroSimNeuronLeakagePower(subArrayHH, adderHH, muxHH, muxDecoderHH, dffHH, subtractorHH);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayHO */
	double heightNeuronHO, widthNeuronHO;
	NeuroSimNeuronArea(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO, &heightNeuronHO, &widthNeuronHO);
	double leakageNeuronHO = NeuroSimNeuronLeakagePower(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO);
	
	/* Print the area of synaptic core and neuron peripheries */
	double totalSubArrayArea = subArrayIH->usedArea + subArrayHO->usedArea;
	double totalNeuronAreaIH = adderIH.area + muxIH.area + muxDecoderIH.area + dffIH.area + subtractorIH.area;
	double totalNeuronAreaHH = adderHH.area + muxHH.area + muxDecoderHH.area + dffHH.area + subtractorHH.area;
	double totalNeuronAreaHO = adderHO.area + muxHO.area + muxDecoderHO.area + dffHO.area + subtractorHO.area;
	//printf("Total SubArray (synaptic core) area=%.4e m^2\n", totalSubArrayArea);
	//printf("Total Neuron (neuron peripheries) area=%.4e m^2\n", totalNeuronAreaIH + totalNeuronAreaHH + totalNeuronAreaHO);
	//printf("Total area=%.4e m^2\n", totalSubArrayArea + totalNeuronAreaIH + totalNeuronAreaHH + totalNeuronAreaHO);
	
	std::cout << param->nHide << std::endl; std::cout << param->nHide2 << std::endl; std::cout << param->alpha1 << std::endl; std::cout << param->alpha2 << std::endl; std::cout << param->alpha3 << std::endl; std::cout << param->optimization_type << std::endl;
	
	/* Print the standby leakage power of synaptic core and neuron peripheries */
	//printf("Leakage power of subArrayIH is : %.4e W\n", subArrayIH->leakage);
	//printf("Leakage power of subArrayHH is : %.4e W\n", subArrayHH->leakage);
	//printf("Leakage power of subArrayHO is : %.4e W\n", subArrayHO->leakage);
	//printf("Leakage power of NeuronIH is : %.4e W\n", leakageNeuronIH);
	//printf("Leakage power of NeuronHH is : %.4e W\n", leakageNeuronHH);
	//printf("Leakage power of NeuronHO is : %.4e W\n", leakageNeuronHO);
	//printf("Total leakage power of subArray is : %.4e W\n", subArrayIH->leakage + subArrayHH->leakage + subArrayHO->leakage);
	//printf("Total leakage power of Neuron is : %.4e W\n", leakageNeuronIH + leakageNeuronHH + leakageNeuronHO);
	
	/* Initialize weights and map weights to conductances for hardware implementation */
	WeightInitialize();
	if (param->useHardwareInTraining) { WeightToConductance(); }

	srand(0);	// Pseudorandom number seed
	
	ofstream mywriteoutfile;
	mywriteoutfile.open("my_log.csv");    
	for (int i = 1; i <= param->totalNumEpochs / param->interNumEpochs; i++) {
		//cout << "Training Epoch : " << i << endl;
		Train(param->numTrainImagesPerEpoch, param->interNumEpochs, param->optimization_type);
		if (!param->useHardwareInTraining && param->useHardwareInTestingFF) { WeightToConductance(); }
		Validate();
		mywriteoutfile << i * param->interNumEpochs << ", " << (double)correct / param->numMnistTestImages * 100 << endl;

		printf("%.2f%\n", (double)correct / param->numMnistTestImages * 100);
		//printf("%.2f%", (double)weight1[10][10]);		printf("  %.2f% ", (double)weight1[20][20]); 		printf("  %.2f%", (double)weight1[30][30]); printf("  %.2f%\n", (double)weight1[40][40]);
		//printf("%.2f%", (double)weight2[1][1]);		printf("  %.2f%", (double)weight2[2][2]); 		printf("  %.2f%", (double)weight2[3][3]); printf("  %.2f%\n", (double)weight2[4][4]);
		//printf("%.2f%", (double)weight3[1][1]);		printf("  %.2f%", (double)weight3[2][2]); 		printf("  %.2f%", (double)weight3[3][3]); printf("  %.2f%\n", (double)weight3[4][4]);

		if (i >param->totalNumEpochs-5) { A += correct; }
		if (i == param->totalNumEpochs) { std::cout << "  " << 20 * A / param->numMnistTestImages << std::endl; }
	}
	printf("\n");
	return 0;
}


