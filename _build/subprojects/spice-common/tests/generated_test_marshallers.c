/* this is a file autogenerated by spice_codegen.py */
/*
  Copyright (C) 2013 Red Hat, Inc.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#include <config.h>
#include "test-marshallers.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <spice/protocol.h>
#include <spice/macros.h>
#include "common/marshaller.h"

#ifdef _MSC_VER
#pragma warning(disable:4101)
#pragma warning(disable:4018)
#endif

SPICE_GNUC_UNUSED static void spice_marshall_array_uint64(SpiceMarshaller *m, uint64_t *ptr, unsigned count)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    uint32_t i;

    for (i = 0; i < count; i++) {
        spice_marshaller_add_uint64(m, *ptr++);
    }
}

void spice_marshall_msg_main_ShortDataSubMarshall(SPICE_GNUC_UNUSED SpiceMarshaller *m, SPICE_GNUC_UNUSED SpiceMsgMainShortDataSubMarshall *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainShortDataSubMarshall *src;
    src = (SpiceMsgMainShortDataSubMarshall *)msg;

    spice_marshaller_add_uint8(m, src->dummy_byte);
    spice_marshaller_add_uint32(m, src->data_size);
    m2 = spice_marshaller_get_ptr_submarshaller(m);
    if (src->data != NULL) {
        spice_marshall_array_uint64(m2, src->data, src->data_size);
    }
}

void spice_marshall_msg_main_ArrayMessage(SPICE_GNUC_UNUSED SpiceMarshaller *m, SPICE_GNUC_UNUSED SpiceMsgMainArrayMessage *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
}

void spice_marshall_msg_main_Zeroes(SPICE_GNUC_UNUSED SpiceMarshaller *m, SPICE_GNUC_UNUSED SpiceMsgMainZeroes *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainZeroes *src;
    src = (SpiceMsgMainZeroes *)msg;

    spice_marshaller_add_uint8(m, 0);
    spice_marshaller_add_uint16(m, src->n);
    spice_marshaller_add_uint32(m, 0);
}

void spice_marshall_msg_main_channels_list(SPICE_GNUC_UNUSED SpiceMarshaller *m, SPICE_GNUC_UNUSED SpiceMsgChannels *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgChannels *src;
    uint16_t *channels__element;
    uint32_t i;
    src = (SpiceMsgChannels *)msg;

    spice_marshaller_add_uint32(m, src->num_of_channels);
    channels__element = src->channels;
    for (i = 0; i < src->num_of_channels; i++) {
        spice_marshaller_add_uint16(m, *channels__element);
        channels__element++;
    }
}

void spice_marshall_msg_main_LenMessage(SPICE_GNUC_UNUSED SpiceMarshaller *m, SPICE_GNUC_UNUSED SpiceMsgMainLenMessage *msg)
{
    SPICE_GNUC_UNUSED SpiceMarshaller *m2;
    SpiceMsgMainLenMessage *src;
    uint32_t *dummy__element;
    uint32_t i;
    src = (SpiceMsgMainLenMessage *)msg;

    dummy__element = src->dummy;
    for (i = 0; i < 2; i++) {
        spice_marshaller_add_uint32(m, *dummy__element);
        dummy__element++;
    }
    /* Remaining data must be appended manually */
}
