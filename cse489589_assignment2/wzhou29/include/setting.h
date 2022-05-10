#ifndef SETTING_H_
#define SETTING_H_

#include "simulator.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#define A 0
#define B 1
#define BufferSize 10000
#define FileSize 20


bool Status; // false for waiting ACK; true for waiting LAYERS
int ASeqnum, BSeqnum, LastSeqnum, LastAcknum, NextSeqnum, i, idx, iterate, Base, N, SendBase, RecvBase, SendTime[BufferSize], A_ACK[BufferSize], B_ACK[BufferSize];
struct pkt BufferRecvPacket[BufferSize];
struct pkt BufferPacket[BufferSize];
struct pkt LastPacket; //if no ACK is received this packet will be send

#endif