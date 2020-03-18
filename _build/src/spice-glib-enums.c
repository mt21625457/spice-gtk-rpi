
/* This file is generated by glib-mkenums, do not modify it. This code is licensed under the same license as the containing project. Note that it links to GLib, so must comply with the LGPL linking clauses. */

#include "spice-glib-enums.h"
#include "spice-channel.h"
#include "channel-inputs.h"
#include "spice-session.h"

#define C_ENUM(v) ((gint) v)
#define C_FLAGS(v) ((guint) v)

/* enumerations from "channel-inputs.h" */

GType
spice_inputs_lock_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SPICE_INPUTS_SCROLL_LOCK), "SPICE_INPUTS_SCROLL_LOCK", "scroll-lock" },
    { C_FLAGS(SPICE_INPUTS_NUM_LOCK), "SPICE_INPUTS_NUM_LOCK", "num-lock" },
    { C_FLAGS(SPICE_INPUTS_CAPS_LOCK), "SPICE_INPUTS_CAPS_LOCK", "caps-lock" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_flags_register_static (g_intern_static_string ("SpiceInputsLock"), values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* enumerations from "spice-channel.h" */

GType
spice_channel_event_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SPICE_CHANNEL_NONE), "SPICE_CHANNEL_NONE", "none" },
    { C_ENUM(SPICE_CHANNEL_OPENED), "SPICE_CHANNEL_OPENED", "opened" },
    { C_ENUM(SPICE_CHANNEL_SWITCHING), "SPICE_CHANNEL_SWITCHING", "switching" },
    { C_ENUM(SPICE_CHANNEL_CLOSED), "SPICE_CHANNEL_CLOSED", "closed" },
    { C_ENUM(SPICE_CHANNEL_ERROR_CONNECT), "SPICE_CHANNEL_ERROR_CONNECT", "error-connect" },
    { C_ENUM(SPICE_CHANNEL_ERROR_TLS), "SPICE_CHANNEL_ERROR_TLS", "error-tls" },
    { C_ENUM(SPICE_CHANNEL_ERROR_LINK), "SPICE_CHANNEL_ERROR_LINK", "error-link" },
    { C_ENUM(SPICE_CHANNEL_ERROR_AUTH), "SPICE_CHANNEL_ERROR_AUTH", "error-auth" },
    { C_ENUM(SPICE_CHANNEL_ERROR_IO), "SPICE_CHANNEL_ERROR_IO", "error-io" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static (g_intern_static_string ("SpiceChannelEvent"), values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* enumerations from "spice-session.h" */

GType
spice_session_verify_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GFlagsValue values[] = {
    { C_FLAGS(SPICE_SESSION_VERIFY_PUBKEY), "SPICE_SESSION_VERIFY_PUBKEY", "pubkey" },
    { C_FLAGS(SPICE_SESSION_VERIFY_HOSTNAME), "SPICE_SESSION_VERIFY_HOSTNAME", "hostname" },
    { C_FLAGS(SPICE_SESSION_VERIFY_SUBJECT), "SPICE_SESSION_VERIFY_SUBJECT", "subject" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_flags_register_static (g_intern_static_string ("SpiceSessionVerify"), values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

GType
spice_session_migration_get_type (void)
{
  static volatile gsize gtype_id = 0;
  static const GEnumValue values[] = {
    { C_ENUM(SPICE_SESSION_MIGRATION_NONE), "SPICE_SESSION_MIGRATION_NONE", "none" },
    { C_ENUM(SPICE_SESSION_MIGRATION_SWITCHING), "SPICE_SESSION_MIGRATION_SWITCHING", "switching" },
    { C_ENUM(SPICE_SESSION_MIGRATION_MIGRATING), "SPICE_SESSION_MIGRATION_MIGRATING", "migrating" },
    { C_ENUM(SPICE_SESSION_MIGRATION_CONNECTING), "SPICE_SESSION_MIGRATION_CONNECTING", "connecting" },
    { 0, NULL, NULL }
  };
  if (g_once_init_enter (&gtype_id)) {
    GType new_type = g_enum_register_static (g_intern_static_string ("SpiceSessionMigration"), values);
    g_once_init_leave (&gtype_id, new_type);
  }
  return (GType) gtype_id;
}

/* Generated data ends here */

