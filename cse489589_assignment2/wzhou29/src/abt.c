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

/********* STUDENTS WRITE THE NEXT SIX ROUTINES *********/

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


/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
  struct msg message;
{
  int index = 0;
  while(index < FileSize && message.data[index]!='\0') {
    BufferPacket[idx].payload[index] = message.data[index];
    index++;
  }
  BufferPacket[idx].payload[index] = '\0';
  if(idx==0) BufferPacket[idx].seqnum = 0;
  else BufferPacket[idx].seqnum = 1-BufferPacket[idx-1].seqnum;
  BufferPacket[idx].checksum = CheckSum(BufferPacket[idx]);
  idx++;
  if(Status && iterate<idx) {
      LastPacket = BufferPacket[iterate];
      ASeqnum = BufferPacket[iterate].seqnum;
      Status = false;
      starttimer(A,FileSize);
      tolayer3(A,LastPacket);
      iterate++;
  }
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
  struct pkt packet;
{
  // if(LastAcknum == packet.acknum) return;
  if(LastAcknum != packet.acknum){
    if(ASeqnum != packet.acknum) {
		  starttimer(A, FileSize);
		  tolayer3(A, LastPacket);
	  }
    else{
		  stoptimer(A);
		  LastAcknum = packet.acknum;
		  Status = true;
		  if(Status && iterate<idx) {
        LastPacket = BufferPacket[iterate];
        ASeqnum = BufferPacket[iterate].seqnum;
        Status = false;
        starttimer(A,FileSize);
        tolayer3(A,LastPacket);
        iterate++;
      }
	  }
  }
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
  starttimer(A, FileSize);
  tolayer3(A, LastPacket);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
  ASeqnum, iterate, idx = 0;
  Status = true;
  LastAcknum = -1;
}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
  struct pkt packet;
{
	struct pkt ACKPacket;
	if(packet.checksum != CheckSum(packet)) ACKPacket.acknum = -1;
  else if (LastSeqnum == packet.seqnum && packet.seqnum != BSeqnum) ACKPacket.acknum = 1 - BSeqnum;
  else{
    tolayer5(B, packet.payload);
    LastSeqnum = packet.seqnum;
    ACKPacket.acknum = BSeqnum;
  	BSeqnum=1-BSeqnum;
  }
  tolayer3(B, ACKPacket);
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
  BSeqnum = 0;
  LastSeqnum = -1;
}
