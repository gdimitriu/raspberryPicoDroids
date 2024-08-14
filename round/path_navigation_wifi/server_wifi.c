/*
 * moving robot raspberry pico w (WIFI based)
 * 
 * server_wifi.c (the tcp server that receive commands)
 * 
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of raspberryPicoDroids project.

 * raspberryPicoDroids is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * raspberryPicoDroids is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with raspberryPicoDroids; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <string.h>
#include <stdlib.h>

#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>

#include <lwip/pbuf.h>
#include <lwip/tcp.h>

#include "server_wifi.h"
#include "make_move.h"

int bufferIndex = 0;
char *bufferReceive = NULL;
char *bufferSend = NULL;

static volatile bool isStarted;

static TCP_SERVER_T *currentState = NULL;

/*
 * static function headers
 */

static err_t tcp_server_close(void *arg);

/*
 * clean the input buffers
 */
static void makeCleanup() {
#ifdef SERIAL_DEBUG_MODE
	printf("make cleanup begin\n");
	fflush(stdout);
#endif
	memset(currentState->buffer_recv, 0, sizeof(uint8_t)*WIFI_BUFFER);
	currentState->recv_len = 0;
	bufferIndex = 0;
#ifdef SERIAL_DEBUG_MODE
	printf("make cleanup end\n");
	fflush(stdout);
#endif
}


static void tcp_server_err(void *arg, err_t err) {
#ifdef SERIAL_DEBUG_MODE
	printf("error_tcp %d\n",err);
	fflush(stdout);
#endif
	TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
        state->complete = true;
		tcp_server_close(arg);
    }
}

/*
 * function to send data to the client
 */
err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb)
{
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    state->sent_len = 0;
#ifdef SERIAL_DEBUG_MODE	
    printf("Writing %lu bytes to client\n", strlen(state->buffer_sent));
	fflush(stdout);
#endif	
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    err_t err = tcp_write(tpcb, state->buffer_sent, strlen(state->buffer_sent), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
#ifdef SERIAL_DEBUG_MODE		
        printf("Failed to write data %d\n", err);
		fflush(stdout);
#endif		
		state->complete = true;
        return tcp_server_close(arg);
    }
    return ERR_OK;
}


err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
#ifdef SERIAL_DEBUG_MODE
	printf("runn recv on core %d\n",get_core_num());
	fflush(stdout);
#endif	
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (!p) {
        state->complete = true;
        return tcp_server_close(arg);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    if (p->tot_len > 0) {
#ifdef SERIAL_DEBUG_MODE		
        printf("tcp_server_recv %d/%d err %d\n", p->tot_len, state->recv_len, err);
		fflush(stdout);
#endif
        // Receive the buffer
        state->recv_len = pbuf_copy_partial(p, state->buffer_recv, p->tot_len, 0);
#ifdef SERIAL_DEBUG_MODE		
		printf("tcp_server_recv_data %s\n",state->buffer_recv);
		fflush(stdout);
#endif
        tcp_recved(tpcb, p->tot_len);
		bufferIndex = 0;
		int skip = 0;
		int currentCommand = 0;
		for ( int i = 0 ; i < state->recv_len; ++i) {
			if ( state->buffer_recv[i] == '#' ) {
				if ( bufferIndex > 0 ) {
					bufferReceive = bufferReceive + (bufferIndex + skip);
					bufferIndex = currentCommand ;
				}
				else {
					bufferReceive = state->buffer_recv;
					bufferIndex = i;
				}
				makeMove(true,0);
				skip = 1;
				currentCommand = 0;
			} else {
				if ( state->buffer_recv[i] == '\n' || state->buffer_recv[i] == '\r' || state->buffer_recv[i] == ' ' || state->buffer_recv[i] == '\0') {
					skip++;
					continue;
				}
				++currentCommand;
			}
		}
		makeCleanup();
    }
    pbuf_free(p);

	return ERR_OK;
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_OK || client_pcb == NULL) {
#ifdef SERIAL_DEBUG_MODE		
        printf("Failure in accept\n");
		fflush(stdout);
#endif
        state->complete = true;
        tcp_server_close(arg);
		isStarted = false;
        return ERR_VAL;
    }
#ifdef SERIAL_DEBUG_MODE    
    printf("Client connected\n");
#endif

    state->client_pcb = client_pcb;
    tcp_arg(client_pcb, state);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_err(client_pcb, tcp_server_err);

    return ERR_OK;
}

static bool tcp_server_open(void *arg) {
	
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
#ifdef SERIAL_DEBUG_MODE	
    printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), WIFI_TCP_PORT);
#endif	

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
#ifdef SERIAL_DEBUG_MODE		
        printf("failed to create pcb\n");
#endif
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, WIFI_TCP_PORT);
    if (err) {
#ifdef SERIAL_DEBUG_MODE		
        printf("failed to bind to port %u\n", WIFI_TCP_PORT);
#endif
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb) {
#ifdef SERIAL_DEBUG_MODE		
        printf("failed to listen\n");
#endif		
        if (pcb) {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    return true;
}


static err_t tcp_server_close(void *arg) {
#ifdef SERIAL_DEBUG_MODE
	printf("close connection\n");
	fflush(stdout);
#endif
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    err_t err = ERR_OK;
    if (state->client_pcb != NULL) {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        err = tcp_close(state->client_pcb);
        if (err != ERR_OK) {
#ifdef SERIAL_DEBUG_MODE			
            printf("close failed %d, calling abort\n", err);
#endif			
            tcp_abort(state->client_pcb);
            err = ERR_ABRT;
        }
        state->client_pcb = NULL;
    }
    if (state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    isStarted = false;
	bufferReceive = NULL;
	bufferSend = NULL;
	free(state);
	currentState = NULL;
    return err;
}

void initWifi() {
	if ( !isStarted ) {
		currentState = calloc(1, sizeof(TCP_SERVER_T));
		if (!currentState) {
#ifdef SERIAL_DEBUG_MODE			
			printf("failed to allocate state\n");
#endif			
			isStarted = false;
			return;
		} else { 
			isStarted = true;
		}
		if (!tcp_server_open(currentState)) {
			currentState->complete = true;
			tcp_server_close(currentState);
			isStarted = false;
			return;
		}
		bufferReceive = currentState->buffer_recv;
		bufferSend = currentState->buffer_sent;
		while(isStarted) {
			
		}
	}
}


void sendData(char *buffer) {
	tcp_server_send_data(currentState, currentState->client_pcb);
}
