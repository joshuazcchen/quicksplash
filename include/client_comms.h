#ifndef CLIENTCOMMS_H
#define CLIENTCOMMS_H

extern int s_socket;

response c_send(Packet p);
response c_connect(int port, char* addr);
response c_read();
extern Packet active;
extern int ready;

#endif
