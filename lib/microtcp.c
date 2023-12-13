/*
 * microtcp, a lightweight implementation of TCP for teaching,
 * and academic purposes.
 *
 * Copyright (C) 2015-2017  Manolis Surligas <surligas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "microtcp.h"
#include "../utils/crc32.h"

microtcp_sock_t microtcp_socket(int domain, int type, int protocol)
{
  microtcp_sock_t socket;

  // Initialize the microtcp_sock_t object
  if(socket.sd = socket(AF_INET, SOCK_DGRAM, 0) < 0) 
  {
    perror("socket creation failed");
    socket.state = INVALID;
    
    return socket;
  }
    socket.state = CLOSED; // Initial state is CLOSED
    socket.init_win_size = MICROTCP_WIN_SIZE;
    socket.curr_win_size = MICROTCP_WIN_SIZE;
    socket.recvbuf = malloc(MICROTCP_RECVBUF_LEN);
    
    if (socket.recvbuf == NULL) 
    {
      perror("Failed to allocate receive buffer");
      close(socket.sd);
      socket.state = INVALID;
      
      return sock;
    }

    memset(socket.recvbuf, 0, MICROTCP_RECVBUF_LEN);
    socket.buf_fill_level = 0;
    socket.cwnd = MICROTCP_INIT_CWND;
    socket.ssthresh = MICROTCP_INIT_SSTHRESH;
    
    socket.seq_number = 0;
    socket.ack_number = 0;
    
    socket.packets_send = 0;
    socket.packets_received = 0;
    socket.packets_lost = 0;
    
    socket.bytes_send = 0;
    socket.bytes_received = 0;
    socket.bytes_lost = 0;

    return socket;
  
}

int microtcp_bind (microtcp_sock_t *socket, const struct sockaddr *address, socklen_t address_len)
{
  if(bind(socket->sd, address, address_len) < 0)
  {
    perror("bind failed");
    socket->state = INVALID;
    
    return -1;
  }

  socket->state = LISTEN;
  return 0;

}

int microtcp_connect(microtcp_sock_t *socket, const struct sockaddr *address, socklen_t address_len)
{
  microtcp_header_t header;
  char buffer[1024]; // Buffer to receive the packet
  
  // generate a random 32-bit sequence number
  socket->seq_number = rand();
  
  header = malloc(sizeof microtcp_header_t);
  header->seq_number = socket->seq_number;
  header->ack_number = 0;
  header->control = 2;  
  header->window = MICROTCP_WIN_SIZE;

    // Send a SYN packet to the server
    if(sendto(socket->sd, buffer, sizeof(buffer), 0, address, address_len) < 0) 
    {
      perror("sendto failed");
      socket->state = INVALID;
      return -1;
    }

    socket->packets_send++;
    socket->bytes_send += sizeof(buffer);

    // Wait for a SYN-ACK packet from the server
    if(recvfrom(socket->sd, buffer, sizeof(buffer), 0, address, address_len) < 0) 
    {
      perror("recvfrom failed");
      socket->state = INVALID;
      return -1;
    }

    socket->packets_received++;
    socket->bytes_received += sizeof(buffer);

    // Send an ACK packet to the server
    if(sendto(socket->sd, buffer, sizeof(buffer), 0, address, address_len) < 0) 
    {
      perror("sendto failed");
      socket->state = INVALID;
      return -1;
    }

    socket->packets_send++;
    socket->bytes_send += sizeof(buffer);s
    socket->state = ESTABLISHED;
    socket->ack_number = header->seq_number + 1;
    socket->seq_number = header->ack_number;
    
    return 0;
}

int microtcp_accept (microtcp_sock_t *socket, struct sockaddr *address, socklen_t address_len)
{
  char buffer[1024]; // Buffer to receive the packet

    // Wait for a packet from the client
    if (recvfrom(socket->sd, buffer, sizeof(buffer), 0, address, address_len) < 0) 
    {
      perror("recvfrom failed");
      socket->state = INVALID;
      return -1;
    }

    socket->state = ESTABLISHED;
    return 0;
}

int microtcp_shutdown (microtcp_sock_t *socket, int how)
{
    
  if (close(socket->sd) < 0) 
  {
    perror("shutdown failed");
    socket->state = INVALID;
    return -1;
  }
  
  socket->state = CLOSED;
  return 0;
}

ssize_t microtcp_send (microtcp_sock_t *socket, const void *buffer, size_t length, int flags)
{
  /* Your code here */
}

ssize_t microtcp_recv (microtcp_sock_t *socket, void *buffer, size_t length, int flags)
{
  /* Your code here */
}
