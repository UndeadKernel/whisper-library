#define PURPLE_PLUGINS

#include <glib.h>
#include <prpl.h>
#include <notify.h>
#include <plugin.h>
#include <version.h>
#include <server.h>
#include <connection.h>
#include <account.h>
#include <accountopt.h>
#include <whisperLibrary/channelmanager.hpp>


//static whisper_library::ChannelManager* channel_manager = NULL;

static gboolean
plugin_load(PurplePlugin *plugin) {
	purple_notify_message(plugin, PURPLE_NOTIFY_MSG_INFO, "Whisper Library activated!",
		"This is the whisper library plugin :)", NULL, NULL, NULL);

	return TRUE;
}

static const char* whisperLibraryListIcon(PurpleAccount* account, PurpleBuddy* buddy){
	return "facebook";
}

void whisperLibraryClose(PurpleConnection* connection){

}

void whisperLibraryLogin(PurpleAccount* account){

}

int whisperLibrarySendIm(PurpleConnection* connection, const char* who, const char* message, PurpleMessageFlags flags){
	return 0;
}

static void init_plugin(PurplePlugin *plugin){
	//channel_manager = new whisper_library::ChannelManager();
	whisperLibraryAccountOptions();
}

void whisperLibraryAccountOptions(){
	//PurpleAccountOption* option_channel = purple_account_option_int_new(channel_manager->getChannelNames()[0].c_str(), "selected channel", 0);
	//PurpleAccountOption* option_adapter = purple_account_option_int_new(channel_manager->getChannelNames()[0].c_str(), "selected adapter", 0);
	//prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option_channel);
	//prpl_info.protocol_options = g_list_append(prpl_info.protocol_options, option_adapter);
}

static PurplePluginProtocolInfo prpl_info =
{
	(PurpleProtocolOptions) (OPT_PROTO_NO_PASSWORD | OPT_PROTO_REGISTER_NOSCREENNAME), /* options */
	NULL, /* user_splits, initialized in toxprpl_init() */
	NULL, /* protocol_options, initialized in toxprpl_init() */
	NULL, 0, 0, 0, 0, 0, PURPLE_ICON_SCALE_SEND,//NO_BUDDY_ICONS, /* icon spec */
	whisperLibraryListIcon, /* list_icon */
	NULL, /* list_emblem */
	NULL, /* status_text */
	NULL, /* tooltip_text */
	NULL, /* status_types */
	NULL, /* blist_node_menu */
	NULL, /* chat_info */
	NULL, /* chat_info_defaults */
	whisperLibraryLogin, /* login */
	whisperLibraryClose, /* close */
	whisperLibrarySendIm, /* send_im */
	NULL, /* set_info */
	NULL, /* send_typing */
	NULL, /* get_info */
	NULL, /* set_status */
	NULL, /* set_idle */
	NULL, /* change_passwd */
	NULL, /* add_buddy */
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
	NULL, /* load */
	NULL, /* unload */
	NULL, /* destroy */
	NULL, /* ui_info */
	&prpl_info, /* extra_info */
	NULL, /* prefs_info */
	NULL, /* actions */
	NULL, /* padding... */
	NULL,
	NULL,
	NULL,                         
};

PURPLE_INIT_PLUGIN(whisperlibrary, init_plugin, info)
