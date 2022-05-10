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
    // for (i=0;i<FileSize;i++){
    //   if(message.data[i]=='\0') break;
    //   BufferPacket[idx].payload[i]=message.data[i];
    // }
    int index = 0;
    while(index < FileSize && message.data[index]!='\0') {
    BufferPacket[idx].payload[index] = message.data[index];
    index++;
  }
    BufferPacket[idx].payload[index]='\0';
    BufferPacket[idx].seqnum=idx;
    BufferPacket[idx].checksum = CheckSum(BufferPacket[idx]);
    idx++;
    i = NextSeqnum;
    while(i<idx && i<Base+N) {
      tolayer3(A, BufferPacket[NextSeqnum]);
      if(Base == NextSeqnum) starttimer(A, 20);
      i++;
    }
    NextSeqnum = i;
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  if (packet.acknum == Base){
    Base++;
    stoptimer(A);
    if(Base<NextSeqnum){
      starttimer(A,FileSize);
    }
    else{
      i = NextSeqnum;
      while(i<idx && i<Base+N) {
        tolayer3(A, BufferPacket[NextSeqnum]);
        if(Base == NextSeqnum) starttimer(A, 20);
        i++;
      }
      NextSeqnum = i;
    }
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  for(i=Base;i<NextSeqnum;i++){
    tolayer3(A,BufferPacket[i]);
    if(i==Base) starttimer(A,FileSize);
  }
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  Base, NextSeqnum, idx = 0;
  N=getwinsize();
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
  struct pkt ACKPacket;
  if(packet.checksum!=CheckSum(packet) || packet.seqnum!=BSeqnum){
    ACKPacket.acknum = LastSeqnum;
  }
  else{
    LastSeqnum = packet.seqnum;
    tolayer5(B, packet.payload);
    ACKPacket.acknum = BSeqnum;
    BSeqnum++;
  }
  tolayer3(B, ACKPacket);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  BSeqnum = 0;
  LastSeqnum = -1;
}
