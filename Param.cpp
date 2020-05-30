#include <string>
#include "math.h"
#include "Param.h"

Param::Param() {
	/* MNIST dataset */
	numMnistTrainImages = 60000;// # of training images in MNIST
	numMnistTestImages = 10000;	// # of testing images in MNIST

	/* Algorithm parameters */
	numTrainImagesPerEpoch = 8000;	// # of training images per epoch
	totalNumEpochs = 125;	// Total number of epochs
	interNumEpochs = 1;		// Internal number of epochs (print out the results every interNumEpochs)
	nInput = 400;     // # of neurons in input layer
	nHide = 100;      // # of neurons in hidden layer
	nOutput = 10;     // # of neurons in output layer
	alpha1 = 0.4;	// Learning rate for the weights from input to hidden layer
	alpha2 = 0.2;	// Learning rate for the weights from hidden to output layer
	maxWeight = 1;	// Upper bound of weight value
	minWeight = -1;	// Lower bound of weight value
    /*Optimization method
    Available option include: "SGD", "Momentum", "Adagrad", "RMSprop" and "Adam"*/
    optimization_type = "SGD";


	/* Hardware parameters */
	useHardwareInTrainingFF = true;   // Use hardware in the feed forward part of training or not (true: realistic hardware, false: ideal software)
	useHardwareInTrainingWU = true;   // Use hardware in the weight update part of training or not (true: realistic hardware, false: ideal software)
	useHardwareInTraining = useHardwareInTrainingFF || useHardwareInTrainingWU;    // Use hardware in the training or not
	useHardwareInTestingFF = true;    // Use hardware in the feed forward part of testing or not (true: realistic hardware, false: ideal software)
	numBitInput = 1;       // # of bits of the input data (=1 for black and white data)
	numBitPartialSum = 8;  // # of bits of the digital output (partial weighted sum output)
	pSumMaxHardware = pow(2, numBitPartialSum) - 1;   // Max digital output value of partial weighted sum
	numInputLevel = pow(2, numBitInput);  // # of levels of the input data
	numWeightBit = 6;	// # of weight bits (only for pure algorithm, SRAM and digital RRAM hardware)
	BWthreshold = 0.5;	// The black and white threshold for numBitInput=1
	Hthreshold = 0.5;	// The spiking threshold for the hidden layer (da1 in Train.cpp and Test.cpp)
	numColMuxed = 16;	// How many columns share 1 read circuit (for analog RRAM) or 1 S/A (for digital RRAM)
	numWriteColMuxed = 16;	// How many columns share 1 write column decoder driver (for digital RRAM)
	writeEnergyReport = true;	// Report write energy calculation or not
	NeuroSimDynamicPerformance = true; // Report the dynamic performance (latency and energy) in NeuroSim or not
	relaxArrayCellHeight = 0;	// True: relax the array cell height to standard logic cell height in the synaptic array
	relaxArrayCellWidth = 0;	// True: relax the array cell width to standard logic cell width in the synaptic array
	arrayWireWidth = 100;	// Array wire width (nm)
	processNode = 32;	// Technology node (nm)
	clkFreq = 2e9;		// Clock frequency (Hz)

}

