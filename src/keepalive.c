/*
Copyright (c) 2009-2020 Roger Light <roger@atchoo.org>

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.
 
The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.
 
SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#include "config.h"
#include <time.h>
#include "mosquitto_broker_internal.h"


static time_t last_keepalive_check = 0;

/* FIXME - this is the prototype for the future tree/trie based keepalive check implementation. */

int keepalive__add(struct mosquitto *context)
{
	UNUSED(context);

	return MOSQ_ERR_SUCCESS;
}


void keepalive__check(void)
{
	struct mosquitto *context, *ctxt_tmp;
	time_t timeout;

	if(db.contexts_by_sock){
		timeout = (last_keepalive_check + 5 - db.now_s);
		if(timeout <= 0){
			timeout = 5;
		}
		loop__update_next_event(timeout*1000);
	}
	if(last_keepalive_check + 5 <= db.now_s){
		last_keepalive_check = db.now_s;

		/* FIXME - this needs replacing with something more efficient */
		HASH_ITER(hh_sock, db.contexts_by_sock, context, ctxt_tmp){
			if(net__is_connected(context)){
				/* Local bridges never time out in this fashion. */
				if(!(context->keepalive)
						|| context->bridge
						|| db.now_s - context->last_msg_in <= (time_t)(context->keepalive)*3/2){

				}else{
					/* Client has exceeded keepalive*1.5 */
					do_disconnect(context, MOSQ_ERR_KEEPALIVE);
				}
			}
		}
	}
}


int keepalive__remove(struct mosquitto *context)
{
	UNUSED(context);

	return MOSQ_ERR_SUCCESS;
}


void keepalive__remove_all(void)
{
}


int keepalive__update(struct mosquitto *context)
{
	context->last_msg_in = db.now_s;
	return MOSQ_ERR_SUCCESS;
}