// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef FLUX_TXDB_LEVELDB_H
#define FLUX_TXDB_LEVELDB_H

#include "main.h"
#include "leveldb.h"

/** CFluxView backed by the LevelDB Flux database (chainstate/) */
class CFluxViewDB : public CFluxView
{
protected:
    CLevelDB db;
public:
    CFluxViewDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    bool GetFlux(const uint256 &txid, CFlux &Flux);
    bool SetFlux(const uint256 &txid, const CFlux &Flux);
    bool HaveFlux(const uint256 &txid);
    CBlockIndex *GetBestBlock();
    bool SetBestBlock(CBlockIndex *pindex);
    bool BatchWrite(const std::map<uint256, CFlux> &mapFlux, CBlockIndex *pindex);
    bool GetStats(CFluxStats &stats);
};

/** Access to the block database (blocks/index/) */
class CBlockTreeDB : public CLevelDB
{
public:
    CBlockTreeDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);
private:
    CBlockTreeDB(const CBlockTreeDB&);
    void operator=(const CBlockTreeDB&);
public:
    bool WriteBlockIndex(const CDiskBlockIndex& blockindex);
    bool ReadBestInvalidWork(CBigNum& bnBestInvalidWork);
    bool WriteBestInvalidWork(const CBigNum& bnBestInvalidWork);
    bool ReadBlockFileInfo(int nFile, CBlockFileInfo &fileinfo);
    bool WriteBlockFileInfo(int nFile, const CBlockFileInfo &fileinfo);
    bool ReadLastBlockFile(int &nFile);
    bool WriteLastBlockFile(int nFile);
    bool WriteReindexing(bool fReindex);
    bool ReadReindexing(bool &fReindex);
    bool ReadTxIndex(const uint256 &txid, CDiskTxPos &pos);
    bool WriteTxIndex(const std::vector<std::pair<uint256, CDiskTxPos> > &list);
    bool WriteFlag(const std::string &name, bool fValue);
    bool ReadFlag(const std::string &name, bool &fValue);
    bool LoadBlockIndexGuts();
};

#endif // FLUX_TXDB_LEVELDB_H
