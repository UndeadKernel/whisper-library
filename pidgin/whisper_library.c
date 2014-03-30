#define PURPLE_PLUGINS

#include <glib.h>
#include <string.h>
#include <prpl.h>
#include <notify.h>
#include <plugin.h>
#include <version.h>
#include <server.h>
#include <connection.h>
#include <account.h>
#include <debug.h>
#include <accountopt.h>
#include <status.h>
#include "whisperLibrary/whisper_library_wrapper.hpp"

static const char* PLUGIN_ID = "prpl_whisper_library";
PurpleStatus* m_status = NULL;
PurpleAccount* m_account = NULL;

static gboolean plugin_load(PurplePlugin *plugin) {
	purple_debug_info(PLUGIN_ID, "plugin_load started\n");
    return TRUE;
}

static const char* wl_list_icon (PurpleAccount* account, PurpleBuddy* buddy){
	return "facebook";
}

void wl_close (PurpleConnection* connection){
	purple_debug_info(PLUGIN_ID, "close called\n");
}


void wl_login (PurpleAccount* account){
	purple_debug_info(PLUGIN_ID, "login called\n");
	m_account = account;
	PurpleConnection* pc = purple_account_get_connection(account);
	purple_debug_info("whisperLibrary", "Trying to log in!\n");
	GHashTable* options = NULL;
	options = account->settings;
	wlSetAdapter((char*)g_hash_table_lookup(options, "selected adapter"));
	purple_connection_set_state(pc, PURPLE_CONNECTED);
	purple_debug_info("whisperLibrary", "WhisperLibrary logged in!\n");
}

int wl_send_im (PurpleConnection* connection, const char* who, const char* message, PurpleMessageFlags flags){
	purple_debug_info(PLUGIN_ID, "send_im called\n");
	purple_debug_info(PLUGIN_ID, "sending message to: ");
	purple_debug_info(PLUGIN_ID, who);
	purple_debug_info(PLUGIN_ID, "message text: ");
	purple_debug_info(PLUGIN_ID, message);
	wlSendMessage(who, message);
	return 1;
}

void wl_set_status(PurpleAccount* account, PurpleStatus* status){
	purple_debug_info(PLUGIN_ID, "set_status called\n");
	m_status = status;
}

static GList* wl_status_types(PurpleAccount* account){
	purple_debug_info(PLUGIN_ID, "status_types called\n");
	GList* types = NULL;
	PurpleStatusType* status;
	status = purple_status_type_new_full(PURPLE_STATUS_AVAILABLE, "wl_online", "Online", FALSE, TRUE, FALSE);
	types = g_list_append(types, status);
	status = purple_status_type_new_full(PURPLE_STATUS_OFFLINE, NULL, "Offline", FALSE, TRUE, FALSE);
	return g_list_append(types, status);
}

void wl_add_buddy(PurpleConnection* connection, PurpleBuddy* buddy, PurpleGroup* group){
	purple_debug_info(PLUGIN_ID, "add_buddy called\n");
	GHashTable* options = NULL;
	options = m_account->settings;
	PurpleContact* contact = purple_buddy_get_contact(buddy);
	if (contact == NULL){
		 purple_debug_info(PLUGIN_ID, "contact = NULL");
	}else{
		purple_blist_add_buddy(buddy, contact, group, NULL);
		wlAddBuddy(buddy->name, (char*)g_hash_table_lookup(options, "selected channel"));
	}
}

void actionShowAdapters (PurplePluginAction* action){
	purple_debug_info("whisperLibrary", wlListAdapters());
}

void actionShowChannels (PurplePluginAction* action){
	purple_debug_info("whisperLibrary", wlListChannels());	
}

void actionSetOptions (PurplePluginAction* action){
	
}

void actionShowSelectedChannel (PurplePluginAction* action){
	
}

void actionShowSelectedAdapter (PurplePluginAction* action){
	
}
	
static GList* listPluginActions (PurplePlugin* plugin, gpointer context){
	purple_debug_info("whisperLibrary", "Setting up plugin actions!\n");
	GList *list = NULL;
    PurplePluginAction* action = NULL;
	
    action = purple_plugin_action_new("Show available covert channels", actionShowChannels);
    list = g_list_append(list, action);
    
    action = purple_plugin_action_new("Show currently selected covert channel", actionShowSelectedChannel);
	list = g_list_append(list, action);
    
    action = purple_plugin_action_new("Show available adapters", actionShowAdapters);
	list = g_list_append(list, action);
	
	action = purple_plugin_action_new("Show currently selected adapter", actionShowSelectedAdapter);
	list = g_list_append(list, action);
	
	action = purple_plugin_action_new("Set channel options", actionSetOptions);
	list = g_list_append(list, action);

    /* Once the list is complete, we send it to libpurple. */
    purple_debug_info("whisperLibrary", "Done setting up!\n");
    return list;	
}

static PurplePluginProtocolInfo prpl_info =
{
	OPT_PROTO_NO_PASSWORD | OPT_PROTO_REGISTER_NOSCREENNAME, /* options */
    NULL, /* user_splits, initialized in toxprpl_init() */
	NULL, /* protocol_options, initialized in toxprpl_init() */
    NO_BUDDY_ICONS, /* icon spec */
    wl_list_icon, /* list_icon */
    NULL, /* list_emblem */
    NULL, /* status_text */
    NULL, /* tooltip_text */
    wl_status_types, /* status_types */
    NULL, /* blist_node_menu */
    NULL, /* chat_info */
    NULL, /* chat_info_defaults */
    wl_login, /* login */
    wl_close, /* close */
    wl_send_im, /* send_im */
    NULL, /* set_info */
    NULL, /* send_typing */
    NULL, /* get_info */
    wl_set_status, /* set_status */
    NULL, /* set_idle */
    NULL, /* change_passwd */
    wl_add_buddy, /* add_buddy */
    NULL, /* add_buddies */
    NULL, /* remove_buddy */
    NULL, /* remove_buddies */
    NULL, /* add_permit */
    NULL, /* add_deny */
    NULL, /* rem_permit */
    NULL, /* rem_deny */
    NULL, /* set_permit_deny */
    NULL, /* join_chat */
    NULL, /* reject_chat */
    NULL, /* get_chat_name */
    NULL, /* chat_invite */
    NULL, /* chat_leave */
    NULL, /* chat_whisper */
    NULL, /* chat_send */
    NULL, /* keepalive */
    NULL, /* register_user */
    NULL, /* get_cb_info */
    NULL, /* get_cb_away */
    NULL, /* alias_buddy */
    NULL, /* group_buddy */
    NULL, /* rename_group */
    NULL, /* buddy_free */
    NULL, /* convo_closed */
    NULL, /* normalize */
    NULL, /* set_buddy_icon */
    NULL, /* remove_group */
    NULL, /* get_cb_real_name */
    NULL, /* set_chat_topic */
    NULL, /* find_blist_chat */
    NULL, /* roomlist_get_list */
    NULL, /* roomlist_cancel */
    NULL, /* roomlist_expand_category */
    NULL, /* can_receive_file */
    NULL, /* send_file */
    NULL, /* new_xfer */
    NULL, /* offline_message */
    NULL, /* whiteboard_prpl_ops */
    NULL, /* send_raw */
    NULL, /* roomlist_room_serialize */
    NULL, /* unregister_user */
    NULL, /* send_attention */
    NULL, /* get_attention_types */
    sizeof(PurplePluginProtocolInfo), /* struct_size */
    NULL, /* get_account_text_table */
    NULL, /* initiate_media */
    NULL, /* get_media_caps */
    NULL, /* get_moods */
    NULL, /* set_public_alias */
    NULL, /* get_public_alias */
    NULL, /* add_buddy_with_invite */
    NULL /* add_buddies_with_invite */
};

static PurplePluginInfo info = {
    PURPLE_PLUGIN_MAGIC, /* magic */
    PURPLE_MAJOR_VERSION, /* major_version */
    PURPLE_MINOR_VERSION, /* minor_version */
    PURPLE_PLUGIN_PROTOCOL, /* type */
    NULL, /* ui_requirement */
    0, /* flags */
    NULL, /* dependencies */
    PURPLE_PRIORITY_DEFAULT, /* priority */
	"prpl-whisper_library_im", /* id */
    "Whisper Library", /* name */
    "0.1", /* version */
    "Plugin for the whisper library. This plugin can be used to communicate using covert channels.", /* summary */
    "Plugin for the whisper library. This plugin can be used to communicate using covert channels.", /* description */
    "Jan Simon Bunten, Simon Kadel, Martin Oehler, Arne Stuehlmeyer", /* author */
    "www.nopage.de", /* homepage */
    plugin_load, /* load */
    NULL, /* unload */
    NULL, /* destroy */
    NULL, /* ui_info */
    &prpl_info, /* extra_info */
    NULL, /* prefs_info */
    listPluginActions, /* actions */
    NULL, /* padding... */
    NULL,
    NULL,
    NULL,                         
};

static void init_plugin (PurplePlugin *plugin){
	PurpleAccountOption *option;
	PurplePluginInfo *info = plugin->info;
	PurplePluginProtocolInfo *prpl_info = info->extra_info;
	
	option = purple_account_option_string_new("Default adapter", "selected adapter", "Enter your adapter name");
	prpl_info->protocol_options = g_list_append(prpl_info->protocol_options, option);
	
	option = purple_account_option_string_new("Default channel", "selected channel", "Enter a covert channel name");
	prpl_info->protocol_options = g_list_append(prpl_info->protocol_options, option);
	
	option = purple_account_option_string_new("Default channel", "channel options", "(Optional) enter covert channel options here and call the 'set channel options' function!");
	prpl_info->protocol_options = g_list_append(prpl_info->protocol_options, option);
	
	wlMakeChannelManager();
}

PURPLE_INIT_PLUGIN(whisperlibrary, init_plugin, info)
