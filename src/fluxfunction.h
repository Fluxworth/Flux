/* 
 * Copyright (c) 2014 The FoxCoin Foxes
 * Copyright (c) 2014 The Flux Foundation
 */

#ifndef FLUXFUNCTION_H
#define	FLUXFUNCTION_H

double getDifficulty();
double GetEstimatedNextDifficulty();
double getReward();
double getBlockReward(int);
double getBlockDifficulty(int);
double getTxTotalValue(std::string);
double convertFlux(int64);
double getTxFees(std::string);
double getTotalVolume();
double getTotalVolumeN(int);

int getNetworkHashesPS();
int getBlockTime(int);
int getBlocknBits(int);
int getBlockNonce(int);
int blocksInPastHours(int);
int getBlockHashrate(int);
int getBlockTimestamp(int);

std::string getNodeInfo();
std::string getInputs(std::string);
std::string getOutputs(std::string);
std::string getBlockHash(int);
std::string getBlockMerkle(int);

bool addnode(std::string);
bool isFluxbase(std::string);

const CBlockIndex* getBlockIndex(int);

int64 getInputValue(CTransaction, CScript);

#endif	/* FLUXFUNCTION_H */