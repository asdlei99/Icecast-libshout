/* -*- c-basic-offset: 8; -*- */
/* proto_icy.c: Implementation of protocol ICY.
 *
 *  Copyright (C) 2002-2004 the Icecast team <team@icecast.org>,
 *  Copyright (C) 2012-2019 Philipp "ph3-der-loewe" Schafft <lion@lion.leolix.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

#ifdef HAVE_CONFIG_H
#   include <config.h>
#endif

#include <shout/shout.h>
#include "shout_private.h"

static int shout_create_icy_request_poke(shout_t *self, shout_connection_t *connection)
{
    if (shout_queue_printf(connection, "!POKE\nicy-name:libshout server poke request\n\n")) {
        return SHOUTERR_MALLOC;
    } else {
        return SHOUTERR_SUCCESS;
    }
}

static int shout_create_icy_request_real(shout_t *self, shout_connection_t *connection)
{
    const char *bitrate;
    const char *val;
    int         ret;

    bitrate = shout_get_audio_info(self, SHOUT_AI_BITRATE);
	if (!bitrate)
        bitrate = "0";

    ret = SHOUTERR_MALLOC;
    do {
		if (shout_queue_printf(connection, "%s\n", self->password))
            break;
		if (shout_queue_printf(connection, "icy-name:%s\n", shout_get_meta(self, "name")))
            break;
        val = shout_get_meta(self, "url");
		if (shout_queue_printf(connection, "icy-url:%s\n", val ? val : "http://www.icecast.org/"))
            break;
        val = shout_get_meta(self, "irc");
		if (shout_queue_printf(connection, "icy-irc:%s\n", val ? val : ""))
            break;
        val = shout_get_meta(self, "aim");
		if (shout_queue_printf(connection, "icy-aim:%s\n", val ? val : ""))
            break;
        val = shout_get_meta(self, "icq");
		if (shout_queue_printf(connection, "icy-icq:%s\n", val ? val : ""))
            break;
		if (shout_queue_printf(connection, "icy-pub:%i\n", self->public))
            break;
        val = shout_get_meta(self, "genre");
		if (shout_queue_printf(connection, "icy-genre:%s\n", val ? val : "icecast"))
            break;
		if (shout_queue_printf(connection, "icy-br:%s\n\n", bitrate))
            break;

        ret = SHOUTERR_SUCCESS;
    } while (0);

    return ret;
}

shout_connection_return_state_t shout_create_icy_request(shout_t *self, shout_connection_t *connection)
{
    int ret;

    if (connection->server_caps & LIBSHOUT_CAP_GOTCAPS) {
        ret = shout_create_icy_request_real(self, connection);
    } else {
        ret = shout_create_icy_request_poke(self, connection);
    }

    shout_connection_set_error(connection, ret);
    return ret == SHOUTERR_SUCCESS ? SHOUT_RS_DONE : SHOUT_RS_ERROR;
}

static const shout_protocol_impl_t shout_icy_impl_real = {
    .msg_create = shout_create_icy_request,
    .msg_get = shout_get_xaudiocast_response,
    .msg_parse = shout_parse_xaudiocast_response
};
const shout_protocol_impl_t *shout_icy_impl = &shout_icy_impl_real;
