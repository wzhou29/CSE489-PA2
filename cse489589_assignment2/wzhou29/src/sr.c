#include "../include/setting.h"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
**********************************************************************/

int CheckSum(packet)
  struct pkt packet;
{
  int checksum = packet.seqnum + packet.acknum;
  int index = 0;
  while(index < FileSize && packet.payload[index]!='\0') {
    checksum+=packet.payload[index];
    index++;
  }
  return checksum;
}


/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
  if (idx != BufferSize){
    BufferPacket[idx].seqnum = NextSeqnum;
    int index = 0;
    while(index < FileSize && message.data[index]!='\0') {
      BufferPacket[idx].payload[index] = message.data[index];
      index++;
    }
    BufferPacket[idx].payload[index] = '\0';
    BufferPacket[idx].checksum = CheckSum(BufferPacket[idx]);
    SendTime[idx] = 0;
    idx++;
    if(NextSeqnum > SendBase && NextSeqnum < (SendBase+N)){
      tolayer3(0, BufferPacket[NextSeqnum]);
      SendTime[NextSeqnum] = get_sim_time();
    }
    else if(SendBase == NextSeqnum){
      tolayer3(0, BufferPacket[NextSeqnum]);
      starttimer(0,25);
    }
    NextSeqnum++;
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  if(packet.acknum >= SendBase){
    if (packet.acknum == SendBase){
      A_ACK[SendBase] = 1;
      stoptimer(0);
      int index = SendBase;
      while(A_ACK[index] == 0 && (index+N) < NextSeqnum && A_ACK[index+N] == 0) {
        tolayer3(0, BufferPacket[index+N]);
        SendTime[index+N] = get_sim_time();
        index++;
      }
      SendBase = index;
    }
    else if(packet.acknum < (SendBase+N)) {
      A_ACK[SendBase] = 1;
    }
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  tolayer3(0, BufferPacket[SendBase]);
  SendTime[SendBase] = get_sim_time();
  starttimer(0,25);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  SendBase, NextSeqnum, idx = 0;
  N = getwinsize();
  memset(A_ACK, 0, BufferSize);
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
  struct pkt ACKPacket;
  if (packet.checksum != CheckSum(packet)){
    ACKPacket.acknum = -1;
    tolayer3(B, ACKPacket);
  }
  else if (packet.seqnum >= RecvBase && packet.seqnum < RecvBase + N){
    ACKPacket.acknum = packet.seqnum;
    tolayer3(1, ACKPacket);
    BufferRecvPacket[packet.seqnum] = packet;
    B_ACK[packet.seqnum] = 1;
    int index = RecvBase;
    while(index < RecvBase+N && B_ACK[index]!=0){
      tolayer5(B, BufferRecvPacket[index].payload);
      index++;
    }
    RecvBase = index;
  }
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  RecvBase = 0;
  memset(B_ACK, 0, BufferSize);
}
