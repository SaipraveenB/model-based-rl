#If RL-Glue Core or the C/C++ Codecs are not installed to default locations
#You may need to use set the CFLAGS and LDFLAGS. 

#Current values of these two vars is supposed to be illustrative, not correct for you.
#This is what you would use if you installed the RL-Glue Core with:
# ./configure --prefix=/Users/btanner/rl-glue

CFLAGS  := $(CFLAGS) -fopenmp -std=c++11 -DDBG_LEVEL=5 -Ofast -funroll-loops -march=native
#CFLAGS  := $(CFLAGS) -DPRETRAIN -DPRETRAIN_SAMPLES=200
LDFLAGS := $(LDFLAGS)  -fopenmp
CFLAGS  := $(CFLAGS) -I/home/bpraveen/rlglue/include

CXX := clang
SIMULATOR := simulateQ
GENERATOR := psr

all: MSRBMTestExperiment CRBMTestExperiment ActualExperiment	

MSRBMTestExperiment: MSRBMTestExperiment.o utils.o putils.o models/model.o generator/generator.o generator/psr/psr.o generator/onehut/onehut.o models/msrbm/msrbm.o simulator/simulate.o models/crbm/layers/conv.o models/crbm/crbm.o
	glibtool --mode=link $(CXX)  $(LDFLAGS) $^ -o $@

CRBMTestExperiment: CRBMTestExperiment.o utils.o putils.o models/model.o generator/generator.o generator/psr/psr.o generator/onehut/onehut.o models/msrbm/msrbm.o simulator/simulate.o models/crbm/layers/conv.o models/crbm/crbm.o
	glibtool --mode=link $(CXX)  $(LDFLAGS) $^ -o $@

ActualExperiment: ActualExperiment.o DynaQAgent.o SkeletonEnvironment.o utils.o putils.o models/model.o generator/generator.o generator/psr/psr.o generator/onehut/onehut.o models/msrbm/msrbm.o simulator/simulate.o models/crbm/layers/conv.o models/crbm/crbm.o
	glibtool --mode=link $(CXX)  $^ /home/bpraveen/rlglue/lib/librl* $(LDFLAGS) -o $@

ActualExperiment.o: ActualExperiment.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

SkeletonEnvironment.o: SkeletonEnvironment.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

MSRBMTestExperiment.o: MSRBMTestExperiment.cpp
	$(CXX)  $(CFLAGS) -c MSRBMTestExperiment.cpp -o MSRBMTestExperiment.o 

CRBMTestExperiment.o: CRBMTestExperiment.cpp
	$(CXX)  $(CFLAGS) -c CRBMTestExperiment.cpp -o CRBMTestExperiment.o 

utils.o: utils.cpp
	$(CXX)  $(CFLAGS) -c utils.cpp -o utils.o 
putils.o: putils.cpp
	$(CXX)  $(CFLAGS) -c putils.cpp -o putils.o 

generator/generator.o: generator/generator.cpp
	$(CXX)  $(CFLAGS) -c generator/generator.cpp -o generator/generator.o

generator/onehut/onehut.o: generator/onehut/onehut.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

generator/psr/psr.o: generator/psr/$(GENERATOR).cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

simulator/simulate.o: simulator/$(SIMULATOR).cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

models/model.o: models/model.cpp
	$(CXX)  $(CFLAGS) -c models/model.cpp -o models/model.o

models/msrbm/msrbm.o: models/msrbm/msrbm.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

models/crbm/crbm.o: models/crbm/crbm.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

models/crbm/layers/conv.o: models/crbm/layers/conv.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

MCGradientAgent.o: MCGradientAgent.cpp
	$(CXX)  $(CFLAGS) -c MCGradientAgent.cpp -o MCGradientAgent.o 

models/simulator/simulator.o: models/simulator/simulator.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@

DynaQAgent.o: DynaQAgent.cpp
	$(CXX)  $(CFLAGS) -c $^ -o $@


clean:
	rm -rf MSRBMTestExperiment *.o
	rm generator/*.o
	rm generator/psr/*.o
	rm models/*.o
	rm models/msrbm/*.o
	rm models/crbm/*.o
	rm models/crbm/layers/*.o
	rm simulator/*.o
