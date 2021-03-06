/**************************************************************************
*                                                                         *
*   Flush - GTK-based BitTorrent client                                   *
*   http://sourceforge.net/projects/flush                                 *
*                                                                         *
*   Copyright (C) 2009-2010, Dmitry Konishchev                            *
*   http://konishchevdmitry.blogspot.com/                                 *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
**************************************************************************/


#include <map>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserbutton.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/linkbutton.h>
#include <gtkmm/notebook.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/stock.h>
#include <gtkmm/table.h>
#include <gtkmm/treestore.h>
#include <gtkmm/window.h>

#include <mlib/misc.hpp>
#include <mlib/string.hpp>

#include <mlib/gtk/builder.hpp>
#include <mlib/gtk/tree_view.hpp>
#include <mlib/gtk/vbox.hpp>

#include "client_settings.hpp"
#include "common.hpp"
#include "daemon_settings.hpp"
#include "gui_lib.hpp"
#include "ip_filter.hpp"
#include "settings_window.hpp"



// Private -->
namespace Settings_window_aux
{
	class Private
	{
		public:
			struct Gui_misc
			{
				Gtk::CheckButton*	show_speed_in_window_title;
				Gtk::CheckButton*	show_zero_values;
				Gtk::CheckButton*	compact_details_tab;

				Gtk::CheckButton*	show_tray_icon;
				Gtk::Container*		tray_container;
				Gtk::CheckButton*	hide_app_to_tray_at_startup;
				Gtk::CheckButton*	minimize_to_tray;
				Gtk::CheckButton*	close_to_tray;

				Gtk::SpinButton*	gui_update_interval;
				Gtk::SpinButton*	max_log_lines;

				Gtk::CheckButton*	download_completed_notification;
				Gtk::CheckButton*	all_downloads_completed_notification;
			};

			struct Status_bar
			{
				Gtk::CheckButton*	download_speed;
				Gtk::CheckButton*	download_payload_speed;

				Gtk::CheckButton*	upload_speed;
				Gtk::CheckButton*	upload_payload_speed;

				Gtk::CheckButton*	download;
				Gtk::CheckButton*	payload_download;

				Gtk::CheckButton*	upload;
				Gtk::CheckButton*	payload_upload;

				Gtk::CheckButton*	share_ratio;
				Gtk::CheckButton*	failed;
				Gtk::CheckButton*	redundant;
			};

			struct Gui
			{
				Gui_misc	misc;
				Status_bar	status_bar;
			};

			struct Client
			{
				Gui		gui;
			};



			struct Network_misc
			{
				Gtk::RadioButton*	random_port;
				Gtk::RadioButton*	custom_port;
				Gtk::Container*		custom_port_box;
				Gtk::SpinButton*	from_port;
				Gtk::SpinButton*	to_port;
				Gtk::Label*			current_port;

				Gtk::CheckButton*	dht;
				Gtk::CheckButton*	upnp;
				Gtk::CheckButton*	lsd;
				Gtk::CheckButton*	natpmp;
				Gtk::CheckButton*	pex;
				Gtk::CheckButton*	smart_ban;

				Gtk::SpinButton*	download_rate_limit;
				Gtk::SpinButton*	upload_rate_limit;
				Gtk::SpinButton*	max_uploads;
				Gtk::SpinButton*	max_connections;
				Gtk::CheckButton*	ignore_limits_on_local_network;

				Gtk::CheckButton*	use_max_announce_interval;
				Gtk::SpinButton*	max_announce_interval;
			};

			struct Network
			{
				Ip_filter*		ip_filter;
				Network_misc	misc;
			};

			struct Auto_load
			{
				Gtk::CheckButton*		is;
				Gtk::Container*			container;

				Gtk::FileChooserButton*	from;
				Gtk::FileChooserButton*	to;

				Gtk::CheckButton*		copy;
				Gtk::FileChooserButton*	copy_to;

				Gtk::CheckButton*		delete_loaded;
			};

			struct Auto_clean
			{
				Gtk::SpinButton*		max_seeding_time;
				Auto_clean_type			max_seeding_time_type;
				Gtk::Button*			max_seeding_time_type_button;
				Gtk::Label*				max_seeding_time_type_label;

				Gtk::SpinButton*		max_ratio;
				Auto_clean_type			max_ratio_type;
				Gtk::Button*			max_ratio_type_button;
				Gtk::Label*				max_ratio_type_label;

				Gtk::SpinButton*		max_seeding_torrents;
				Auto_clean_type			max_seeding_torrents_type;
				Gtk::Button*			max_seeding_torrents_type_button;
				Gtk::Label*				max_seeding_torrents_type_label;
			};

			struct Automation
			{
				Auto_load	load;
				Auto_clean	clean;
			};

			struct Daemon
			{
				Network		network;
				Automation	automation;
			};


		public:
			Client			client;
			Daemon			daemon;


		public:
			/// Обработчик сигнала на изменение диапазона прослушиваемых портов.
			void	on_listen_port_range_changed_cb(void);

			/// Обработчик сигнала на переключение флажка "случайный
			/// порт"-"задаваемый пользователем порт".
			void	on_random_port_toggled_cb(void);
	};



	void Private::on_listen_port_range_changed_cb(void)
	{
		Network_misc& misc = this->daemon.network.misc;
		misc.from_port->set_range(m::PORT_MIN, misc.to_port->get_value());
	}



	void Private::on_random_port_toggled_cb(void)
	{
		Network_misc& misc = this->daemon.network.misc;
		misc.custom_port_box->set_sensitive(!misc.random_port->get_active());
	}
}
// Private <--



// Sections_view -->
	Settings_window::Sections_view_model_columns::Sections_view_model_columns(void)
	{
		add(this->id);
		add(this->name);
	}



	Settings_window::Sections_view_columns::Sections_view_columns(const Sections_view_model_columns& model_columns)
	:
		name(_("Name"), model_columns.name)
	{
		M_GTK_TREE_VIEW_ADD_STRING_COLUMN(name, _("Name"))
	}
// Sections_view <--



Settings_window::Settings_window(Gtk::Window& parent_window, Client_settings* client_settings, Daemon_settings* daemon_settings)
:
	m::gtk::Dialog(parent_window, _("Preferences")),

	priv(new Private),

	client_settings(*client_settings),
	daemon_settings(*daemon_settings),

	download_to_dialog(
		*this, format_window_title(_("Please select torrents download directory")),
		Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER
	),
	download_to_button(download_to_dialog),

	copy_finished_to_dialog(
		*this, format_window_title(_("Please select directory for finished downloads copying")),
		Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER
	),
	copy_finished_to_button(copy_finished_to_dialog)
{
	const int tabs_border_width = m::gtk::BOX_BORDER_WIDTH * 3;

	this->set_resizable(false);
	this->remove();

	Gtk::VBox* main_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
	this->add(*main_vbox);

	Gtk::HBox* main_hbox = Gtk::manage(new Gtk::HBox(false, m::gtk::HBOX_SPACING));
	main_vbox->pack_start(*main_hbox, true, true);

	std::map<Section, Gtk::TreePath> sections_paths;

	// Sections view -->
	{
		Gtk::Frame* sections_frame = Gtk::manage(new Gtk::Frame());
		main_hbox->pack_start(*sections_frame, true, true);

		sections_frame->add(this->sections_view);

		this->sections_view.set_headers_visible(false);
		this->sections_view.columns.name.property_sizing() = Gtk::TREE_VIEW_COLUMN_AUTOSIZE;
		this->sections_view.get_selection()->set_mode(Gtk::SELECTION_SINGLE);

		Gtk::TreeRow row;
		Gtk::TreeRow client_row;
		Gtk::TreeRow daemon_row;
		Gtk::TreeModel::iterator iter;

		// Добавляем разделы -->
			// Client -->
				iter = this->sections_view.model->append();
				sections_paths[CLIENT] = this->sections_view.model->get_path(iter);
				client_row = *iter;
				client_row[this->sections_view.model_columns.id] = CLIENT;
				client_row[this->sections_view.model_columns.name] = _("Client");

				iter = this->sections_view.model->append(client_row.children());
				sections_paths[CLIENT_MAIN] = this->sections_view.model->get_path(iter);
				row = *iter;
				row[this->sections_view.model_columns.id] = CLIENT_MAIN;
				row[this->sections_view.model_columns.name] = _("Main");

				// GUI -->
				{
					Gtk::TreeRow gui_row;

					iter = this->sections_view.model->append(client_row.children());
					sections_paths[CLIENT_GUI] = this->sections_view.model->get_path(iter);
					gui_row = *iter;
					gui_row[this->sections_view.model_columns.id] = CLIENT_GUI;
					gui_row[this->sections_view.model_columns.name] = _("GUI");

					iter = this->sections_view.model->append(gui_row.children());
					sections_paths[CLIENT_GUI_MISC] = this->sections_view.model->get_path(iter);
					row = *iter;
					row[this->sections_view.model_columns.id] = CLIENT_GUI_MISC;
					row[this->sections_view.model_columns.name] = _("Misc");

					iter = this->sections_view.model->append(gui_row.children());
					sections_paths[CLIENT_GUI_STATUS_BAR] = this->sections_view.model->get_path(iter);
					row = *iter;
					row[this->sections_view.model_columns.id] = CLIENT_GUI_STATUS_BAR;
					row[this->sections_view.model_columns.name] = _("Status bar");
				}
				// GUI <--
			// Client <--


			// Daemon -->
				iter = this->sections_view.model->append();
				sections_paths[DAEMON] = this->sections_view.model->get_path(iter);
				daemon_row = *iter;
				daemon_row[this->sections_view.model_columns.id] = DAEMON;
				daemon_row[this->sections_view.model_columns.name] = _("Daemon");

				// Network -->
				{
					Gtk::TreeRow network_row;

					iter = this->sections_view.model->append(daemon_row.children());
					sections_paths[DAEMON_NETWORK] = this->sections_view.model->get_path(iter);
					network_row = *iter;
					network_row[this->sections_view.model_columns.id] = DAEMON_NETWORK;
					network_row[this->sections_view.model_columns.name] = _("Network");

					iter = this->sections_view.model->append(network_row.children());
					sections_paths[DAEMON_NETWORK_MISC] = this->sections_view.model->get_path(iter);
					row = *iter;
					row[this->sections_view.model_columns.id] = DAEMON_NETWORK_MISC;
					row[this->sections_view.model_columns.name] = _("Misc");

					iter = this->sections_view.model->append(network_row.children());
					sections_paths[DAEMON_NETWORK_IP_FILTER] = this->sections_view.model->get_path(iter);
					row = *iter;
					row[this->sections_view.model_columns.id] = DAEMON_NETWORK_IP_FILTER;
					row[this->sections_view.model_columns.name] = _("IP filter");
				}
				// Network <--

				iter = this->sections_view.model->append(daemon_row.children());
				sections_paths[DAEMON_AUTOMATION] = this->sections_view.model->get_path(iter);
				row = *iter;
				row[this->sections_view.model_columns.id] = DAEMON_AUTOMATION;
				row[this->sections_view.model_columns.name] = _("Automation");
			// Daemon <--
		// Добавляем раздел <--

		this->sections_view.expand_all();

		// Устанавливаем обработчик сигнала на изменение выделенной секции
		this->sections_view.get_selection()->signal_changed().connect(
			sigc::mem_fun(*this, &Settings_window::on_section_changed_callback)
		);
	}
	// Sections view <--

	this->sections_notebook.set_show_tabs(false);
	main_hbox->pack_start(this->sections_notebook, true, true);


	// Client -->
	{
		Gtk::VBox* settings_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
		settings_vbox->set_border_width(tabs_border_width);
		this->sections_notebook.append_page(*settings_vbox);

		Gtk::HBox* hbox;
		Gtk::VBox* vbox;
		Gtk::LinkButton* button;

		m::gtk::vbox::add_big_header(*settings_vbox, _("Client"));

		vbox = Gtk::manage(new Gtk::VBox(false, 0));
		settings_vbox->pack_start(*vbox, false, false);

		// Main -->
			hbox = Gtk::manage(new Gtk::HBox(false, 0));
			vbox->pack_start(*hbox, false, false);

			button = Gtk::manage(new Gtk::LinkButton("Client::Main", _("Main")));
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button,
						sections_paths[CLIENT_MAIN]
					)
				)
			);
			hbox->pack_start(*button, false, false);
		// Main <--

		// GUI -->
			hbox = Gtk::manage(new Gtk::HBox(false, 0));
			vbox->pack_start(*hbox, false, false);

			button = Gtk::manage(new Gtk::LinkButton("Client::GUI", _("GUI")));
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button,
						sections_paths[CLIENT_GUI]
					)
				)
			);
			hbox->pack_start(*button, false, false);
		// GUI <--
	}
	// Client <--


	// Client::Main -->
	{
		Gtk::VBox* settings_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
		settings_vbox->set_border_width(tabs_border_width);
		this->sections_notebook.append_page(*settings_vbox);

		m::gtk::vbox::add_big_header(*settings_vbox, _("Client::Main"));

		// show_add_torrent_dialog -->
			this->show_add_torrent_dialog.set_label(_("Show add torrent dialog when opening torrent file"));
			settings_vbox->pack_start(this->show_add_torrent_dialog, false, false);
		// show_add_torrent_dialog <--

		// start torrent on adding -->
			this->start_torrent_on_adding_check_button.set_label(_("Start torrent on adding"));
			settings_vbox->pack_start(this->start_torrent_on_adding_check_button, false, false);
		// start torrent on adding <--

		m::gtk::vbox::add_space(*settings_vbox);

		// download_to -->
			this->download_to_dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			this->download_to_dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
			this->download_to_dialog.set_default_response(Gtk::RESPONSE_OK);
			m::gtk::vbox::add_widget_with_label(*settings_vbox, _("Download to:"), this->download_to_button);
		// download_to <--

		// copy_finished_to -->
		{
			Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox(false, m::gtk::HBOX_SPACING));
			settings_vbox->pack_start(*hbox, false, false);

			this->copy_finished_to_check_button.set_label( _("Copy finished to:") ),
			this->copy_finished_to_check_button.set_active();
			this->copy_finished_to_check_button.signal_toggled().connect(sigc::mem_fun(*this, &Settings_window::on_copy_finished_to_path_toggled_callback));
			hbox->pack_start(this->copy_finished_to_check_button, false, false);

			this->copy_finished_to_dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
			this->copy_finished_to_dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);
			this->copy_finished_to_dialog.set_default_response(Gtk::RESPONSE_OK);
			hbox->pack_end(this->copy_finished_to_button, false, false);
		}
		// copy_finished_to <--

		m::gtk::vbox::add_space(*settings_vbox);

		// run command
		m::gtk::vbox::add_widget_with_label(*settings_vbox, _("Open command:"), this->open_command, true, true);
	}
	// Client::Main <--


	// Client::GUI -->
	{
		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.client.gui.glade",
			"client_gui_settings"
		);

		Gtk::VBox* settings_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
		settings_vbox->set_border_width(tabs_border_width);
		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "client_gui_settings")
		);

		{
			Gtk::LinkButton* button;

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "client_gui_misc_link", button);
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button, sections_paths[CLIENT_GUI_MISC]
					)
				)
			);

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "client_gui_status_bar_link", button);
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button, sections_paths[CLIENT_GUI_STATUS_BAR]
					)
				)
			);
		}
	}
	// Client::GUI <--


	// Client::GUI::Misc -->
	{
		Private::Gui_misc& misc = priv->client.gui.misc;

		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.client.gui.misc.glade",
			"gui_misc_settings"
		);

		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "gui_misc_settings")
		);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "show_speed_in_window_title",				misc.show_speed_in_window_title);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "show_zero_values",						misc.show_zero_values);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "compact_details_tab",						misc.compact_details_tab);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "show_tray_icon",							misc.show_tray_icon);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "tray_container",							misc.tray_container);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "hide_app_to_tray_at_startup",				misc.hide_app_to_tray_at_startup);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "minimize_to_tray",						misc.minimize_to_tray);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "close_to_tray",							misc.close_to_tray);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "gui_update_interval",						misc.gui_update_interval);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "max_log_lines",							misc.max_log_lines);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "download_completed_notification",			misc.download_completed_notification);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "all_downloads_completed_notification",	misc.all_downloads_completed_notification);


		misc.show_tray_icon->signal_toggled().connect(sigc::mem_fun(
			*this, &Settings_window::on_show_tray_icon_toggled_callback
		));
	}
	// Client::GUI::Misc <--


	// Client::GUI::Status bar -->
	{
		Private::Status_bar& bar = priv->client.gui.status_bar;

		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.client.gui.status_bar.glade",
			"status_bar_settings"
		);

		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "status_bar_settings")
		);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "download_speed",			bar.download_speed);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "download_payload_speed",	bar.download_payload_speed);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "upload_speed",			bar.upload_speed);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "upload_payload_speed",	bar.upload_payload_speed);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "download",				bar.download);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "payload_download",		bar.payload_download);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "upload",					bar.upload);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "payload_upload",			bar.payload_upload);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "share_ratio",				bar.share_ratio);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "failed",					bar.failed);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "redundant",				bar.redundant);
	}
	// Client::GUI::Status bar <--



	// Daemon -->
	{
		Gtk::VBox* settings_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
		settings_vbox->set_border_width(tabs_border_width);
		this->sections_notebook.append_page(*settings_vbox);

		Gtk::HBox* hbox;
		Gtk::VBox* vbox;
		Gtk::LinkButton* button;

		m::gtk::vbox::add_big_header(*settings_vbox, _("Daemon"));

		vbox = Gtk::manage(new Gtk::VBox(false, 0));
		settings_vbox->pack_start(*vbox, false, false);

		// Network -->
			hbox = Gtk::manage(new Gtk::HBox(false, 0));
			vbox->pack_start(*hbox, false, false);

			button = Gtk::manage(new Gtk::LinkButton("Daemon::Network", _("Network")));
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button,
						sections_paths[DAEMON_NETWORK]
					)
				)
			);
			hbox->pack_start(*button, false, false);
		// Network <--

		// Automation -->
			hbox = Gtk::manage(new Gtk::HBox(false, 0));
			vbox->pack_start(*hbox, false, false);

			button = Gtk::manage(new Gtk::LinkButton("Daemon::Automation", _("Automation")));
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button,
						sections_paths[DAEMON_AUTOMATION]
					)
				)
			);
			hbox->pack_start(*button, false, false);
		// Automation <--
	}
	// Daemon <--


	// Daemon::Network -->
	{
		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.daemon.network.glade",
			"daemon_network_settings"
		);

		Gtk::VBox* settings_vbox = Gtk::manage(new Gtk::VBox(false, m::gtk::VBOX_SPACING));
		settings_vbox->set_border_width(tabs_border_width);
		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "daemon_network_settings")
		);

		{
			Gtk::LinkButton* button;

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "daemon_network_misc_link", button);
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button, sections_paths[DAEMON_NETWORK_MISC]
					)
				)
			);

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "daemon_network_ip_filter_link", button);
			button->signal_clicked().connect(
				sigc::bind< std::pair<Gtk::LinkButton*, Gtk::TreePath> >(
					sigc::mem_fun(*this, &Settings_window::on_change_section_callback),
					std::pair<Gtk::LinkButton*, Gtk::TreePath>(
						button, sections_paths[DAEMON_NETWORK_IP_FILTER]
					)
				)
			);
		}
	}
	// Daemon::Network <--


	// Daemon::Network::Misc -->
	{
		Private::Network_misc& misc = priv->daemon.network.misc;

		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.daemon.network.misc.glade",
			"network_misc_settings"
		);

		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "network_misc_settings")
		);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "random_port", misc.random_port);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "custom_port", misc.custom_port);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "custom_port_box", misc.custom_port_box);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "from_port", misc.from_port);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "to_port", misc.to_port);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "current_port", misc.current_port);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "dht", misc.dht);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "upnp", misc.upnp);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "lsd", misc.lsd);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "natpmp", misc.natpmp);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "pex", misc.pex);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "smart_ban", misc.smart_ban);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "download_rate_limit", misc.download_rate_limit);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "upload_rate_limit", misc.upload_rate_limit);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "max_uploads", misc.max_uploads);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "max_connections", misc.max_connections);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "ignore_limits_on_local_network", misc.ignore_limits_on_local_network);

		MLIB_GTK_BUILDER_GET_WIDGET(builder, "use_max_announce_interval", misc.use_max_announce_interval);
		MLIB_GTK_BUILDER_GET_WIDGET(builder, "max_announce_interval", misc.max_announce_interval);

		misc.random_port->signal_toggled().connect(
			sigc::mem_fun(*priv, &Private::on_random_port_toggled_cb));
		misc.from_port->signal_value_changed().connect(
			sigc::mem_fun(*priv, &Private::on_listen_port_range_changed_cb));
		misc.to_port->signal_value_changed().connect(
			sigc::mem_fun(*priv, &Private::on_listen_port_range_changed_cb));
	}
	// Daemon::Network::Misc <--


	// Daemon::IP filter -->
	{
		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.daemon.network.ip_filter.glade",
			"ip_filter_settings"
		);

		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "ip_filter_settings")
		);
		MLIB_GTK_BUILDER_GET_WIDGET_DERIVED(builder, "ip_filter", priv->daemon.network.ip_filter);
	}
	// Daemon::IP filter <--


	// Daemon::Automation -->
	{
		Private::Automation& automation = priv->daemon.automation;

		m::gtk::Builder builder = MLIB_GTK_BUILDER_CREATE(
			std::string(APP_UI_PATH) + "/preferences.daemon.automation.glade",
			"automation_settings"
		);

		this->sections_notebook.append_page(
			*MLIB_GTK_BUILDER_GET_WIDGET(builder, "automation_settings")
		);


		// Auto load -->
		{
			Private::Auto_load& load = automation.load;

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load", load.is);
			load.is->signal_toggled().connect(sigc::mem_fun(
				*this, &Settings_window::on_auto_load_torrents_toggled_callback
			));

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_container", load.container);

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_from", load.from);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_to", load.to);

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_copy", load.copy);
			load.copy->signal_toggled().connect(sigc::mem_fun(
				*this, &Settings_window::on_auto_load_torrents_copy_to_toggled_callback
			));
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_copy_to", load.copy_to);

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_load_delete_loaded", load.delete_loaded);
		}
		// Auto load <--


		// Auto clean -->
		{
			Private::Auto_clean& clean = automation.clean;

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_time_type", clean.max_seeding_time_type_button);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_time_label", clean.max_seeding_time_type_label);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_time", clean.max_seeding_time);
			clean.max_seeding_time_type_button->signal_clicked().connect(sigc::mem_fun(
				*this, &Settings_window::on_auto_clean_max_seeding_time_clicked_cb
			));

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_ratio_type", clean.max_ratio_type_button);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_ratio_label", clean.max_ratio_type_label);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_ratio", clean.max_ratio);
			clean.max_ratio_type_button->signal_clicked().connect(sigc::mem_fun(
				*this, &Settings_window::on_auto_clean_max_ratio_clicked_cb
			));

			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_torrents_type", clean.max_seeding_torrents_type_button);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_torrents_label", clean.max_seeding_torrents_type_label);
			MLIB_GTK_BUILDER_GET_WIDGET(builder, "auto_clean_max_seeding_torrents", clean.max_seeding_torrents);
			clean.max_seeding_torrents_type_button->signal_clicked().connect(sigc::mem_fun(
				*this, &Settings_window::on_auto_clean_max_seeding_torrents_clicked_cb
			));
		}
		// Auto clean <--
	}
	// Daemon::Automation <--

	// OK, Cancel -->
	{
		Gtk::ButtonBox* button_box = Gtk::manage(new Gtk::HButtonBox());
		button_box->set_layout(Gtk::BUTTONBOX_END);
		button_box->set_spacing(m::gtk::HBOX_SPACING);
		main_vbox->pack_start(*button_box, false, false);


		Gtk::Button* button;

		button = Gtk::manage(new Gtk::Button(Gtk::Stock::CANCEL));
		button->signal_clicked().connect(sigc::mem_fun(*this, &Settings_window::on_cancel_button_callback));
		button_box->add(*button);

		button = Gtk::manage(new Gtk::Button(Gtk::Stock::OK));
		button->signal_clicked().connect(sigc::mem_fun(*this, &Settings_window::on_ok_button_callback));
		button_box->add(*button);
	}
	// OK, Cancel <--

	// Закрытие окна
	this->signal_delete_event().connect(sigc::mem_fun(*this, &Settings_window::on_close_callback));

	this->load_settings();

	// Выделяем вкладку по умолчанию -->
		this->show_all_children(); // Иначе сигнал не будет сгенерирован
		this->sections_view.get_selection()->select(sections_paths[CLIENT_MAIN]);
	// Выделяем вкладку по умолчанию <--

	this->show_all();
}



Settings_window::~Settings_window(void)
{
	delete this->priv;
}



void Settings_window::auto_clean_widgets_update_for(const Auto_clean_type& type, Gtk::Button* type_button, Gtk::Label* type_label, Gtk::Widget* widget)
{
	type_button->set_image(
		*Gtk::manage(new Gtk::Image( type.to_stock_id(), Gtk::ICON_SIZE_BUTTON ))
	);
	type_label->set_sensitive(type);
	widget->set_sensitive(type);
}



void Settings_window::close(void)
{
	if(this->download_to_dialog.is_visible())
		this->download_to_dialog.response(Gtk::RESPONSE_CANCEL);

	this->hide();
}



void Settings_window::load_settings(void)
{
	// Client -->
	{
		Private::Client& client = priv->client;
		const Client_settings& client_settings = this->client_settings;

		// GUI -->
		{
			Private::Gui& gui = client.gui;
			const Gui_settings& gui_settings = client_settings.gui;

			// Misc -->
			{
				Private::Gui_misc& misc = gui.misc;

				misc.show_speed_in_window_title->set_active(			gui_settings.show_speed_in_window_title);
				misc.show_zero_values->set_active(						gui_settings.show_zero_values);
				misc.compact_details_tab->set_active(					gui_settings.compact_details_tab);

				misc.gui_update_interval->set_value(					gui_settings.update_interval);
				misc.max_log_lines->set_value(							gui_settings.max_log_lines);

				misc.show_tray_icon->set_active(						gui_settings.show_tray_icon);
				misc.hide_app_to_tray_at_startup->set_active(			gui_settings.hide_app_to_tray_at_startup);
				misc.minimize_to_tray->set_active(						gui_settings.minimize_to_tray);
				misc.close_to_tray->set_active(							gui_settings.close_to_tray);

				misc.download_completed_notification->set_active(		gui_settings.download_completed_notification);
				misc.all_downloads_completed_notification->set_active(	gui_settings.all_downloads_completed_notification);
			}
			// Misc <--

			// Status bar -->
			{
				Private::Status_bar& bar = gui.status_bar;
				const Status_bar_settings& bar_settings = gui_settings.main_window.status_bar;

				bar.download_speed->set_active(			bar_settings.download_speed);
				bar.download_payload_speed->set_active(	bar_settings.download_payload_speed);

				bar.upload_speed->set_active(			bar_settings.upload_speed);
				bar.upload_payload_speed->set_active(	bar_settings.upload_payload_speed);

				bar.download->set_active(				bar_settings.download);
				bar.payload_download->set_active(		bar_settings.payload_download);

				bar.upload->set_active(					bar_settings.upload);
				bar.payload_upload->set_active(			bar_settings.payload_upload);

				bar.share_ratio->set_active(			bar_settings.share_ratio);
				bar.failed->set_active(					bar_settings.failed);
				bar.redundant->set_active(				bar_settings.redundant);
			}
			// Status bar <--
		}
		// GUI <--

		// Main -->
			this->show_add_torrent_dialog.set_active(this->client_settings.gui.show_add_torrent_dialog);

			this->start_torrent_on_adding_check_button.set_active(this->client_settings.user.start_torrent_on_adding);

			this->download_to_button.set_current_folder(U2L(this->client_settings.user.download_to));

			if(this->client_settings.user.copy_finished_to != "")
			{
				this->copy_finished_to_check_button.set_active();
				this->copy_finished_to_button.set_current_folder(U2L(this->client_settings.user.copy_finished_to));
			}
			else
				this->copy_finished_to_check_button.set_active(false);

			this->open_command.set_text(this->client_settings.user.open_command);
		// Main <--
	}
	// Client <--

	// daemon settings -->
	{
		Daemon_settings& settings = this->daemon_settings;

		// Network -->
			// Misc -->
			{
				Private::Network_misc& misc = priv->daemon.network.misc;

				misc.random_port	->set_active(settings.listen_random_port);
				// Сначала задаем to, чтобы обработчик сигнала выставил ограничение для from
				misc.to_port		->set_value(settings.listen_ports_range.second);
				misc.from_port		->set_value(settings.listen_ports_range.first);
				misc.custom_port	->set_active(!settings.listen_random_port);

				misc.current_port->set_label(
					settings.listen_port < m::PORT_MIN ? _("none") : m::to_string(settings.listen_port));

				misc.dht		->set_active(settings.dht);
				misc.lsd		->set_active(settings.lsd);
				misc.upnp		->set_active(settings.upnp);
				misc.natpmp		->set_active(settings.natpmp);
				misc.pex		->set_active(settings.pex);
				misc.smart_ban	->set_active(settings.smart_ban);

				misc.download_rate_limit			->set_value(settings.download_rate_limit);
				misc.upload_rate_limit				->set_value(settings.upload_rate_limit);
				misc.max_uploads					->set_value(settings.max_uploads);
				misc.max_connections				->set_value(settings.max_connections);
				misc.ignore_limits_on_local_network	->set_active(settings.ignore_limits_on_local_network);

				misc.use_max_announce_interval	->set_active(settings.use_max_announce_interval);
				misc.max_announce_interval		->set_value(settings.max_announce_interval / 60);
			}
			// Misc <--

			// IP filter -->
				priv->daemon.network.ip_filter->set_enabled(settings.ip_filter_enabled);
				priv->daemon.network.ip_filter->set(settings.ip_filter);
			// IP filter <--
		// Network <--

		// Automation -->
		{
			Private::Automation& automation = priv->daemon.automation;

			// Auto load -->
			{
				Daemon_settings::Torrents_auto_load& auto_load = daemon_settings.torrents_auto_load;

				automation.load.is->set_active(auto_load.is);
				automation.load.from->set_current_folder(U2L(auto_load.from));
				automation.load.to->set_current_folder(U2L(auto_load.to));
				automation.load.copy->set_active(auto_load.copy);
				automation.load.copy_to->set_current_folder(U2L(auto_load.copy_to));
				automation.load.delete_loaded->set_active(auto_load.delete_loaded);
			}
			// Auto load <--

			// Auto clean -->
			{
				Daemon_settings::Auto_clean& daemon_clean = daemon_settings.torrents_auto_clean;
				Private::Auto_clean& clean = automation.clean;

				clean.max_seeding_time_type = daemon_clean.max_seeding_time_type;
				clean.max_seeding_time->set_value(daemon_clean.max_seeding_time / 60);
				this->auto_clean_widgets_update_for(
					clean.max_seeding_time_type,
					clean.max_seeding_time_type_button,
					clean.max_seeding_time_type_label,
					clean.max_seeding_time
				);

				clean.max_ratio_type = daemon_clean.max_ratio_type;
				clean.max_ratio->set_value(daemon_clean.max_ratio);
				this->auto_clean_widgets_update_for(
					clean.max_ratio_type,
					clean.max_ratio_type_button,
					clean.max_ratio_type_label,
					clean.max_ratio
				);

				clean.max_seeding_torrents_type = daemon_clean.max_seeding_torrents_type;
				clean.max_seeding_torrents->set_value(daemon_clean.max_seeding_torrents);
				this->auto_clean_widgets_update_for(
					clean.max_seeding_torrents_type,
					clean.max_seeding_torrents_type_button,
					clean.max_seeding_torrents_type_label,
					clean.max_seeding_torrents
				);
			}
			// Auto clean <--
		}
		// Automation <--
	}
	// daemon settings <--
}



void Settings_window::on_auto_clean_max_ratio_clicked_cb(void)
{
	Private::Auto_clean& clean = priv->daemon.automation.clean;

	this->auto_clean_widgets_update_for(
		++clean.max_ratio_type,
		  clean.max_ratio_type_button,
		  clean.max_ratio_type_label,
		  clean.max_ratio
	);
}



void Settings_window::on_auto_clean_max_seeding_time_clicked_cb(void)
{
	Private::Auto_clean& clean = priv->daemon.automation.clean;

	this->auto_clean_widgets_update_for(
		++clean.max_seeding_time_type,
		  clean.max_seeding_time_type_button,
		  clean.max_seeding_time_type_label,
		  clean.max_seeding_time
	);
}



void Settings_window::on_auto_clean_max_seeding_torrents_clicked_cb(void)
{
	Private::Auto_clean& clean = priv->daemon.automation.clean;

	this->auto_clean_widgets_update_for(
		++clean.max_seeding_torrents_type,
		  clean.max_seeding_torrents_type_button,
		  clean.max_seeding_torrents_type_label,
		  clean.max_seeding_torrents
	);
}



void Settings_window::on_auto_load_torrents_toggled_callback(void)
{
	Private::Auto_load& load = priv->daemon.automation.load;

	load.container->set_sensitive(load.is->get_active());
	load.from->set_sensitive(load.is->get_active());
}



void Settings_window::on_auto_load_torrents_copy_to_toggled_callback(void)
{
	Private::Auto_load& load = priv->daemon.automation.load;
	load.copy_to->set_sensitive(load.copy->get_active());
}



void Settings_window::on_cancel_button_callback(void)
{
	this->close();
	this->response(Gtk::RESPONSE_CANCEL);
}



void Settings_window::on_change_section_callback(const std::pair<Gtk::LinkButton*, Gtk::TreePath>& target)
{
	g_object_set(target.first->gobj(), "visited", false, NULL);
	this->sections_view.get_selection()->select(target.second);
}



bool Settings_window::on_close_callback(GdkEventAny* event)
{
	this->close();
	this->response(Gtk::RESPONSE_CANCEL);
	return true;
}



void Settings_window::on_copy_finished_to_path_toggled_callback(void)
{
	this->copy_finished_to_button.set_sensitive(this->copy_finished_to_check_button.get_active());
}



void Settings_window::on_ok_button_callback(void)
{
	this->save_settings();
	this->close();
	this->response(Gtk::RESPONSE_OK);
}



void Settings_window::on_section_changed_callback(void)
{
	Glib::RefPtr<Gtk::TreeView::Selection> selection = this->sections_view.get_selection();

	// При сворачивании строк в одну выделение снимается (несмотря на то, что
	// для Gtk::TreeView::Selection установлен режим Gtk::SELECTION_SINGLE).
	if(selection->count_selected_rows())
	{
		this->sections_notebook.set_current_page(
			selection->get_selected()->get_value(
				this->sections_view.model_columns.id
			)
		);
	}
}



void Settings_window::on_show_tray_icon_toggled_callback(void)
{
	Private::Gui_misc& misc = priv->client.gui.misc;
	misc.tray_container->set_sensitive(misc.show_tray_icon->get_active());
}



void Settings_window::save_settings(void)
{
	// Client -->
	{
		Private::Client& client = priv->client;
		Client_settings& client_settings = this->client_settings;

		// GUI -->
		{
			Private::Gui& gui = client.gui;
			Gui_settings& gui_settings = client_settings.gui;

			// Misc -->
			{
				Private::Gui_misc& misc = gui.misc;

				gui_settings.show_speed_in_window_title				= misc.show_speed_in_window_title->get_active();
				gui_settings.show_zero_values						= misc.show_zero_values->get_active();
				gui_settings.compact_details_tab					= misc.compact_details_tab->get_active();

				gui_settings.update_interval						= misc.gui_update_interval->get_value();
				gui_settings.max_log_lines							= misc.max_log_lines->get_value();

				gui_settings.show_tray_icon							= misc.show_tray_icon->get_active();
				gui_settings.hide_app_to_tray_at_startup			= misc.hide_app_to_tray_at_startup->get_active();
				gui_settings.minimize_to_tray						= misc.minimize_to_tray->get_active();
				gui_settings.close_to_tray							= misc.close_to_tray->get_active();

				gui_settings.download_completed_notification		= misc.download_completed_notification->get_active();
				gui_settings.all_downloads_completed_notification	= misc.all_downloads_completed_notification->get_active();
			}
			// Misc <--

			// Status bar -->
			{
				Private::Status_bar& bar = gui.status_bar;
				Status_bar_settings& bar_settings = gui_settings.main_window.status_bar;

				bar_settings.download_speed				= bar.download_speed->get_active();
				bar_settings.download_payload_speed		= bar.download_payload_speed->get_active();

				bar_settings.upload_speed				= bar.upload_speed->get_active();
				bar_settings.upload_payload_speed		= bar.upload_payload_speed->get_active();

				bar_settings.download					= bar.download->get_active();
				bar_settings.payload_download			= bar.payload_download->get_active();

				bar_settings.upload						= bar.upload->get_active();
				bar_settings.payload_upload				= bar.payload_upload->get_active();

				bar_settings.share_ratio				= bar.share_ratio->get_active();
				bar_settings.failed						= bar.failed->get_active();
				bar_settings.redundant					= bar.redundant->get_active();
			}
			// Status bar <--
		}
		// GUI <--

		// Main -->
			this->client_settings.gui.show_add_torrent_dialog = this->show_add_torrent_dialog.get_active();
			this->client_settings.user.start_torrent_on_adding = this->start_torrent_on_adding_check_button.get_active();

			this->client_settings.user.download_to = L2U(this->download_to_button.get_filename());

			if(this->copy_finished_to_check_button.get_active())
				this->client_settings.user.copy_finished_to = L2U(this->copy_finished_to_button.get_filename());
			else
				this->client_settings.user.copy_finished_to = "";

			this->client_settings.user.open_command = this->open_command.get_text();
			if(m::is_empty_string(this->client_settings.user.open_command))
				this->client_settings.user.open_command = "";
		// Main <--
	}
	// Client <--


	// Daemon -->
	{
		Daemon_settings& settings = this->daemon_settings;

		// Network -->
			// Misc -->
			{
				Private::Network_misc& misc = priv->daemon.network.misc;

				settings.listen_random_port = misc.random_port->get_active();
				settings.listen_ports_range = std::pair<int, int>(
					misc.from_port->get_value(), misc.to_port->get_value());

				settings.dht		= misc.dht->get_active();
				settings.lsd		= misc.lsd->get_active();
				settings.upnp		= misc.upnp->get_active();
				settings.natpmp		= misc.natpmp->get_active();
				settings.smart_ban	= misc.smart_ban->get_active();
				settings.pex		= misc.pex->get_active();

				settings.download_rate_limit			= misc.download_rate_limit->get_value();
				settings.upload_rate_limit				= misc.upload_rate_limit->get_value();
				settings.max_uploads					= misc.max_uploads->get_value();
				settings.max_connections				= misc.max_connections->get_value();
				settings.ignore_limits_on_local_network	= misc.ignore_limits_on_local_network->get_active();

				settings.use_max_announce_interval	= misc.use_max_announce_interval->get_active();
				settings.max_announce_interval		= misc.max_announce_interval->get_value() * 60;
			}
			// Misc <--

			// IP filter -->
				settings.ip_filter_enabled = priv->daemon.network.ip_filter->get_enabled();
				settings.ip_filter = priv->daemon.network.ip_filter->get();
			// IP filter <--
		// Network <--

		// Automation -->
			// Torrents auto load -->
			{
				Daemon_settings::Torrents_auto_load& auto_load = settings.torrents_auto_load;
				Private::Auto_load& load = priv->daemon.automation.load;

				auto_load.is			= load.is->get_active();
				auto_load.from			= L2U(load.from->get_filename());
				auto_load.to			= L2U(load.to->get_filename());

				auto_load.copy			= load.copy->get_active();
				auto_load.copy_to		= L2U(load.copy_to->get_filename());

				auto_load.delete_loaded	= load.delete_loaded->get_active();
			}
			// Torrents auto load <--

			// Torrents auto clean -->
			{
				Daemon_settings::Auto_clean& auto_clean = settings.torrents_auto_clean;
				Private::Auto_clean& clean = priv->daemon.automation.clean;

				auto_clean.max_seeding_time				= clean.max_seeding_time->get_value() * 60;
				auto_clean.max_seeding_time_type		= clean.max_seeding_time_type;

				auto_clean.max_ratio					= clean.max_ratio->get_value();
				auto_clean.max_ratio_type				= clean.max_ratio_type;

				auto_clean.max_seeding_torrents			= clean.max_seeding_torrents->get_value();
				auto_clean.max_seeding_torrents_type	= clean.max_seeding_torrents_type;
			}
			// Torrents auto clean <--
		// Automation <--
	}
	// Daemon <--
}

