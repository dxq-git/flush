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


#ifndef HEADER_DAEMON_TYPES
#define HEADER_DAEMON_TYPES

#include <libtorrent/torrent_handle.hpp>

#include "common.hpp"
#include "daemon_settings.hpp"


#define TORRENT_FILE_NAME "torrent.torrent"



class Torrent
{
	public:
		Torrent(
			const Torrent_full_id& full_id,
			const lt::torrent_handle& handle,
			const m::lt::Torrent_metadata& torrent_metadata,
			const Torrent_settings& settings
		);


	public:
		Torrent_id							id;

		/// За время существования сессии в нее могут добавляться и
		/// удаляться торренты с одинаковыми идентификаторами. Данный
		/// идентификатор позволяет однозначно идентифицировать данный
		/// торрент среди других торрентов с такими же идентификаторами,
		/// которые были до него.
		size_t								serial_number;

		std::string							name;
		std::string							magnet;
		lt::torrent_handle					handle;

		/// Кодировка *.torrent файла.
		std::string							encoding;

		/// URL, с которого был скачан торрент.
		std::string							publisher_url;


		/// "Кэш" состояния раздачи, обновляемого каждый
		/// раз при обновлении статистики торрента.
		bool								seeding;


		/// Время, когда торрент был добавлен в сессию.
		time_t								time_added;

		/// Время в секундах, в течении которого торрент
		/// находился в состоянии seeding.
		time_t								time_seeding;


		/// Текущая ревизия файлов торрента.
		Revision							files_revision;

		/// Настройки файлов торрента.
		std::vector<Torrent_file_settings>	files_settings;


		/// Текущая ревизия списка трекеров.
		Revision							trackers_revision;

		/// Последняя ошибка соединения с трекером, полученная от
		/// libtorrent.
		std::string							tracker_error;


		/// Текущая ревизия параметров скачивания торрента.
		Revision							download_settings_revision;

		/// Параметры скачивания торрента.
		Download_settings_light				download_settings;


		/// Общий объем полученной информации.
		/// Информация из прошлой сессии.
		Size								total_download;

		/// Общий объем скачанных данных (полезная нагрузка).
		/// Информация из прошлой сессии.
		Size								total_payload_download;

		/// Общий объем отправленной информации.
		/// Информация из прошлой сессии.
		Size								total_upload;

		/// Общий объем отправленных данных (полезная нагрузка).
		/// Информация из прошлой сессии.
		Size								total_payload_upload;

		/// Общий объем скачанных данных, которые оказались поврежденными (мусор).
		/// Информация из прошлой сессии.
		Size								total_failed;

		/// Общий объем данных, которые были приняты от пиров, несмотря на то, что
		/// уже были скачаны. Это может возникнуть из-за задержек в работе сети.
		/// Информация из прошлой сессии.
		Size								total_redundant;


		/// Размер скачанных данных, который был в прошлой сессии.
		Size								bytes_done;

		/// Размер скачанных данных, который был в последний раз, когда
		/// libtorrent сгенерировал для торрента torrent_finished_alert.
		Size								bytes_done_on_last_torrent_finish;


	public:
		/// Возвращает путь к директории, в которую будет скачивать торрент.
		std::string			get_download_path(void) const;

		/// Возвращает пареметры скачивания торрента.
		Download_settings	get_download_settings(void) const;

		/// Возвращает полный идентификатор данного торрента.
		Torrent_full_id		get_full_id(void) const;

		/// Возвращает информацию о торренте.
		Torrent_info		get_info(void) const;

		/// Определяет, принадлежит ли торрент к указанной группе.
		bool				is_belong_to(Torrents_group group) const;

		/// Возвращает текущее состояние: приостановлен или нет.
		bool				is_paused(void) const;

		/// Обработчик сигнала на получение данных торрента (при скачивании
		/// через magnet-ссылку).
		void				on_metadata_received(const std::string& settings_dir_path);

		/// Передает текущие настройки файлов libtorrent.
		void				sync_files_settings(void);
};



class Daemon_statistics
{
	public:
		Daemon_statistics(void);


	public:
		/// Время начала текущей сессии.
		Time	session_start_time;

		/// Время начала сбора статистической информации.
		Time	statistics_start_time;

		/// Объем полученной информации за эту сессию.
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	download;

		/// Объем скачанных данных за эту сессию (полезная нагрузка).
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	payload_download;

		/// Общий объем полученной информации.
		Size	total_download;

		/// Общий объем скачанных данных (полезная нагрузка).
		Size	total_payload_download;

		/// Объем отправленной информации за эту сессию.
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	upload;

		/// Объем отправленных данных за эту сессию (полезная нагрузка).
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	payload_upload;

		/// Общий объем отправленной информации.
		Size	total_upload;

		/// Общий объем отправленных данных (полезная нагрузка).
		Size	total_payload_upload;

		/// Объем скачанных данных, которые оказались поврежденными (мусор).
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	failed;

		/// Общий объем скачанных данных, которые оказались поврежденными (мусор).
		Size	total_failed;

		/// Объем данных, которые были приняты от пиров, несмотря на то, что
		/// уже были скачаны. Это может возникнуть из-за задержек в работе сети.
		/// (разница между тем значением, которое следует отобразить
		/// пользователю, и тем, которое выдает libtorrent).
		Size	redundant;

		/// Общий объем данных, которые были приняты от пиров, несмотря на то, что
		/// уже были скачаны. Это может возникнуть из-за задержек в работе сети.
		Size	total_redundant;


	public:
		/// Дает счетчикам такие значения, чтобы при получении на их основе
		/// статистики сессии (Session_status) все значения были нули.
		void	reset(const lt::session_status& libtorrent_session_status);
};

#endif

