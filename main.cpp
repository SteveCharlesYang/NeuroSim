#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <random>
#include <vector>

#include "lib/cxxopts.hpp"
#include "lib/utils.hpp"
#include "lib/argparser.hpp"

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

int main(int argc, char** argv) {
    cxxopts::ParseResult opt = initialize_options(argc, argv);

    if (opt.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }

	gen.seed(0);

	ReadTrainingDataFromFile(str2char(opt["trfp"].as<std::string>()), str2char(opt["trfl"].as<std::string>()));
	ReadTestingDataFromFile(str2char(opt["tefp"].as<std::string>()), str2char(opt["tefl"].as<std::string>()));

    switch (DeviceTypeMap[opt["IHDevice"].as<std::string>()]){
        case Device_Ideal: arrayIH->Initialization<IdealDevice>(1, false, opt, 1); break;
        case Device_Real: arrayIH->Initialization<RealDevice>(1, false, opt, 1); break;
        case Device_Measured: arrayIH->Initialization<MeasuredDevice>(1, false, opt, 1); break;
        case Device_SRAM: arrayIH->Initialization<SRAM>(param->numWeightBit, false, opt, 1); break;
        case Device_DigitalNVM: arrayIH->Initialization<DigitalNVM>(param->numWeightBit,true, opt, 1); break;
    }

    switch (DeviceTypeMap[opt["HODevice"].as<std::string>()]){
        case Device_Ideal: arrayHO->Initialization<IdealDevice>(1, false, opt, 0); break;
        case Device_Real: arrayHO->Initialization<RealDevice>(1, false, opt, 0); break;
        case Device_Measured: arrayHO->Initialization<MeasuredDevice>(1, false, opt, 0); break;
        case Device_SRAM: arrayHO->Initialization<SRAM>(param->numWeightBit, false, opt, 0); break;
        case Device_DigitalNVM: arrayHO->Initialization<DigitalNVM>(param->numWeightBit,true, opt, 0); break;
    }

    param->numMnistTrainImages = opt["MNISTTrainImgs"].as<int>();
    param->numMnistTestImages = opt["MNISTTestImgs"].as<int>();
    param->numTrainImagesPerEpoch = opt["TrainImgEpoch"].as<int>();
    param->totalNumEpochs = opt["totEpoch"].as<int>();
    param->interNumEpochs = opt["intEpoch"].as<int>();
    param->nInput = opt["nIn"].as<int>();
    param->nHide = opt["nHide"].as<int>();
    param->nOutput = opt["nOut"].as<int>();
    param->alpha1 = opt["alp1"].as<double>();
    param->alpha2 = opt["alp2"].as<double>();
    param->maxWeight = opt["maxW"].as<double>();
    param->minWeight = opt["minW"].as<double>();
    param->optimization_type = str2char(opt["opt"].as<std::string>());
    param->useHardwareInTrainingFF = opt["HWTrainFF"].as<bool>();
    param->useHardwareInTrainingWU = opt["HWTrainWU"].as<bool>();
    param->useHardwareInTraining = param->useHardwareInTrainingFF || param->useHardwareInTrainingWU;
    param->useHardwareInTestingFF = opt["HWTestFF"].as<bool>();
    param->numBitInput = opt["nBitInput"].as<int>();
    param->numBitPartialSum = opt["nBitPartial"].as<int>();
    param->pSumMaxHardware = pow(2, param->numBitPartialSum) - 1;
    param->numInputLevel = pow(2, param->numBitInput);
    param->numWeightBit = opt["nWBit"].as<int>();
    param->BWthreshold = opt["BWthres"].as<double>();
    param->Hthreshold = opt["Hthres"].as<double>();
    param->numColMuxed = opt["nColMux"].as<int>();
    param->numWriteColMuxed = opt["nWriteColMux"].as<int>();
    param->writeEnergyReport = opt["writeEReport"].as<bool>();
    param->NeuroSimDynamicPerformance = opt["SimDynPerf"].as<bool>();
    param->relaxArrayCellHeight = opt["relaxArrCellH"].as<bool>();
    param->relaxArrayCellWidth = opt["relaxArrCellW"].as<bool>();
    param->arrayWireWidth = opt["arrWireW"].as<double>();
    param->processNode = opt["node"].as<int>();
    param->clkFreq = opt["clkfq"].as<double>();

	/* Initialization of NeuroSim synaptic cores */
	param->relaxArrayCellWidth = 0;
	NeuroSimSubArrayInitialize(subArrayIH, arrayIH, inputParameterIH, techIH, cellIH);
	param->relaxArrayCellWidth = 1;
	NeuroSimSubArrayInitialize(subArrayHO, arrayHO, inputParameterHO, techHO, cellHO);
	/* Calculate synaptic core area */
	NeuroSimSubArrayArea(subArrayIH);
	NeuroSimSubArrayArea(subArrayHO);

	/* Calculate synaptic core standby leakage power */
	NeuroSimSubArrayLeakagePower(subArrayIH);
	NeuroSimSubArrayLeakagePower(subArrayHO);

	/* Initialize the neuron peripheries */
	NeuroSimNeuronInitialize(subArrayIH, inputParameterIH, techIH, cellIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH);
	NeuroSimNeuronInitialize(subArrayHO, inputParameterHO, techHO, cellHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayIH */
	double heightNeuronIH, widthNeuronIH;
	NeuroSimNeuronArea(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH, &heightNeuronIH, &widthNeuronIH);
	double leakageNeuronIH = NeuroSimNeuronLeakagePower(subArrayIH, adderIH, muxIH, muxDecoderIH, dffIH, subtractorIH);
	/* Calculate the area and standby leakage power of neuron peripheries below subArrayHO */
	double heightNeuronHO, widthNeuronHO;
	NeuroSimNeuronArea(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO, &heightNeuronHO, &widthNeuronHO);
	double leakageNeuronHO = NeuroSimNeuronLeakagePower(subArrayHO, adderHO, muxHO, muxDecoderHO, dffHO, subtractorHO);

	/* Print the area of synaptic core and neuron peripheries */
	double totalSubArrayArea = subArrayIH->usedArea + subArrayHO->usedArea;
	double totalNeuronAreaIH = adderIH.area + muxIH.area + muxDecoderIH.area + dffIH.area + subtractorIH.area;
	double totalNeuronAreaHO = adderHO.area + muxHO.area + muxDecoderHO.area + dffHO.area + subtractorHO.area;
	printf("Total SubArray (synaptic core) area=%.4e m^2\n", totalSubArrayArea);
	printf("Total Neuron (neuron peripheries) area=%.4e m^2\n", totalNeuronAreaIH + totalNeuronAreaHO);
	printf("Total area=%.4e m^2\n", totalSubArrayArea + totalNeuronAreaIH + totalNeuronAreaHO);

	/* Print the standby leakage power of synaptic core and neuron peripheries */
	printf("Leakage power of subArrayIH is : %.4e W\n", subArrayIH->leakage);
	printf("Leakage power of subArrayHO is : %.4e W\n", subArrayHO->leakage);
	printf("Leakage power of NeuronIH is : %.4e W\n", leakageNeuronIH);
	printf("Leakage power of NeuronHO is : %.4e W\n", leakageNeuronHO);
	printf("Total leakage power of subArray is : %.4e W\n", subArrayIH->leakage + subArrayHO->leakage);
	printf("Total leakage power of Neuron is : %.4e W\n", leakageNeuronIH + leakageNeuronHO);
	/* Initialize weights and map weights to conductances for hardware implementation */
	WeightInitialize();
	if (param->useHardwareInTraining) { WeightToConductance(); }
	srand(0);	// Pseudorandom number seed
	
	ofstream mywriteoutfile;
	mywriteoutfile.open("my_log.csv");
	for (int i=1; i<=param->totalNumEpochs/param->interNumEpochs; i++) {
        //cout << "Training Epoch : " << i << endl;
        char* opt_type = str2char(param->optimization_type);
		Train(param->numTrainImagesPerEpoch, param->interNumEpochs,opt_type);
		if (!param->useHardwareInTraining && param->useHardwareInTestingFF) { WeightToConductance(); }
		Validate();
		mywriteoutfile << i*param->interNumEpochs << ", " << (double)correct/param->numMnistTestImages*100 << endl;
		
		printf("Accuracy at %d epochs is : %.2f%\n", i*param->interNumEpochs, (double)correct/param->numMnistTestImages*100);
		printf("\tRead latency=%.4e s\n", subArrayIH->readLatency + subArrayHO->readLatency);
		printf("\tWrite latency=%.4e s\n", subArrayIH->writeLatency + subArrayHO->writeLatency);
		printf("\tRead energy=%.4e J\n", arrayIH->readEnergy + subArrayIH->readDynamicEnergy + arrayHO->readEnergy + subArrayHO->readDynamicEnergy);
		printf("\tWrite energy=%.4e J\n", arrayIH->writeEnergy + subArrayIH->writeDynamicEnergy + arrayHO->writeEnergy + subArrayHO->writeDynamicEnergy);
	}
	printf("\n");
	return 0;
}


