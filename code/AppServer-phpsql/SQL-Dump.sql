-- phpMyAdmin SQL Dump
-- version 5.1.2
-- https://www.phpmyadmin.net/
--
-- Erstellungszeit: 30. Jan 2022 um 00:17

-- 30.01.2022   JM	vereinfacht und für Workshop angepasst 

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Datenbank: `darchamgrouplorawan`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `daten`
--

CREATE TABLE `daten` (
  `id` int(11) NOT NULL,
  `zeit` datetime NOT NULL,
  `devEUI` varchar(32) DEFAULT NULL,
  `json` text NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Daten für Tabelle `daten`
--

INSERT INTO `daten` (`id`, `zeit`, `devEUI`, `json`, `timestamp`) VALUES
(1, '2022-01-30 00:17:05', '70B3D57ED004A71F', '{\"end_device_ids\":{\"device_id\":\"eui-70b3d57ed004a71f\",\"application_ids\":{\"application_id\":\"darc-hamgroup-ttgo-lora32-temp\"},\"dev_eui\":\"70B3D57ED004A71F\",\"join_eui\":\"0000000000000000\",\"dev_addr\":\"260BCAFE\"},\"correlation_ids\":[\"as:up:01FTM21G2KQV54S8TV6B83W7QZ\",\"gs:conn:01FTHSANZ9GQCA0FSH2BE7AYMY\",\"gs:up:host:01FTHSANZZXPPDWP7Y2GKZAHBW\",\"gs:uplink:01FTM21FW0GD84FMJG5Z64DJF8\",\"ns:uplink:01FTM21FW1FHR0AM1ERDQWZ4YP\",\"rpc:/ttn.lorawan.v3.GsNs/HandleUplink:01FTM21FW1YYY17Q5E6GA6ZRE8\",\"rpc:/ttn.lorawan.v3.NsAs/HandleUplink:01FTM21G2K9DGM2MX46GEMHBB0\"],\"received_at\":\"2022-01-29T23:17:05.748553694Z\",\"uplink_message\":{\"session_key_id\":\"AX6f44JL1bOeVY0MIVUIJg==\",\"f_port\":1,\"f_cnt\":9744,\"frm_payload\":\"BK0=\",\"decoded_payload\":{\"bytes\":[4,173],\"temperatur\":19.7},\"rx_metadata\":[{\"gateway_ids\":{\"gateway_id\":\"mikrotik-wap-lora8-kit-211024\",\"eui\":\"343632383F006F00\"},\"time\":\"2022-01-29T23:17:05.484880Z\",\"timestamp\":3123989620,\"rssi\":-69,\"channel_rssi\":-69,\"snr\":11,\"location\":{\"latitude\":48.820698154652284,\"longitude\":9.382012777368077,\"source\":\"SOURCE_REGISTRY\"},\"uplink_token\":\"CisKKQodbWlrcm90aWstd2FwLWxvcmE4LWtpdC0yMTEwMjQSCDQ2Mjg/AG8AEPSY0dELGgwI8ZXXjwYQgJDj/wEgoMqq4vWkEioMCPGV148GEIDdmucB\"}],\"settings\":{\"data_rate\":{\"lora\":{\"bandwidth\":125000,\"spreading_factor\":8}},\"coding_rate\":\"4/5\",\"frequency\":\"868100000\",\"timestamp\":3123989620,\"time\":\"2022-01-29T23:17:05.484880Z\"},\"received_at\":\"2022-01-29T23:17:05.537923107Z\",\"consumed_airtime\":\"0.092672s\",\"network_ids\":{\"net_id\":\"000013\",\"tenant_id\":\"ttn\",\"cluster_id\":\"ttn-eu1\"}}}', '2022-01-29 23:17:05'),
(2, '2022-01-30 00:17:20', '70B3D57ED004A71F', '{\"end_device_ids\":{\"device_id\":\"eui-70b3d57ed004a71f\",\"application_ids\":{\"application_id\":\"darc-hamgroup-ttgo-lora32-temp\"},\"dev_eui\":\"70B3D57ED004A71F\",\"join_eui\":\"0000000000000000\",\"dev_addr\":\"260BCAFE\"},\"correlation_ids\":[\"as:up:01FTM21Y3XJJSRRTAY5MTVP2TR\",\"gs:conn:01FTHSANZ9GQCA0FSH2BE7AYMY\",\"gs:up:host:01FTHSANZZXPPDWP7Y2GKZAHBW\",\"gs:uplink:01FTM21XXE5C34RT9Q63FD07FK\",\"ns:uplink:01FTM21XXFDBGKMAS6VY202G0J\",\"rpc:/ttn.lorawan.v3.GsNs/HandleUplink:01FTM21XXF131ZDG056VJFXFCA\",\"rpc:/ttn.lorawan.v3.NsAs/HandleUplink:01FTM21Y3W095BS0ZYJFA10TN6\"],\"received_at\":\"2022-01-29T23:17:20.126547728Z\",\"uplink_message\":{\"session_key_id\":\"AX6f44JL1bOeVY0MIVUIJg==\",\"f_port\":1,\"f_cnt\":9745,\"frm_payload\":\"BK0=\",\"decoded_payload\":{\"bytes\":[4,173],\"temperatur\":19.7},\"rx_metadata\":[{\"gateway_ids\":{\"gateway_id\":\"mikrotik-wap-lora8-kit-211024\",\"eui\":\"343632383F006F00\"},\"time\":\"2022-01-29T23:17:19.857399Z\",\"timestamp\":3138362260,\"rssi\":-68,\"channel_rssi\":-68,\"snr\":9,\"location\":{\"latitude\":48.820698154652284,\"longitude\":9.382012777368077,\"source\":\"SOURCE_REGISTRY\"},\"uplink_token\":\"CisKKQodbWlrcm90aWstd2FwLWxvcmE4LWtpdC0yMTEwMjQSCDQ2Mjg/AG8AEJS3vtgLGgwI/5XXjwYQtOr0tQMgoPTep6ulEioMCP+V148GENi965gD\",\"channel_index\":2}],\"settings\":{\"data_rate\":{\"lora\":{\"bandwidth\":125000,\"spreading_factor\":8}},\"coding_rate\":\"4/5\",\"frequency\":\"868500000\",\"timestamp\":3138362260,\"time\":\"2022-01-29T23:17:19.857399Z\"},\"received_at\":\"2022-01-29T23:17:19.919771046Z\",\"consumed_airtime\":\"0.092672s\",\"network_ids\":{\"net_id\":\"000013\",\"tenant_id\":\"ttn\",\"cluster_id\":\"ttn-eu1\"}}}', '2022-01-29 23:17:20'),
(3, '2022-01-30 00:17:34', '70B3D57ED004A71F', '{\"end_device_ids\":{\"device_id\":\"eui-70b3d57ed004a71f\",\"application_ids\":{\"application_id\":\"darc-hamgroup-ttgo-lora32-temp\"},\"dev_eui\":\"70B3D57ED004A71F\",\"join_eui\":\"0000000000000000\",\"dev_addr\":\"260BCAFE\"},\"correlation_ids\":[\"as:up:01FTM22C59WTQ96A7QHVC7CQZ0\",\"gs:conn:01FTHSANZ9GQCA0FSH2BE7AYMY\",\"gs:up:host:01FTHSANZZXPPDWP7Y2GKZAHBW\",\"gs:uplink:01FTM22BYJ9296GMD19BDWABZE\",\"ns:uplink:01FTM22BYK4YTRPG42E4KAEG7N\",\"rpc:/ttn.lorawan.v3.GsNs/HandleUplink:01FTM22BYKSEMXHMDX24382B8H\",\"rpc:/ttn.lorawan.v3.NsAs/HandleUplink:01FTM22C58VJ1ZTBZYRSBZXEXD\"],\"received_at\":\"2022-01-29T23:17:34.506281071Z\",\"uplink_message\":{\"session_key_id\":\"AX6f44JL1bOeVY0MIVUIJg==\",\"f_port\":1,\"f_cnt\":9746,\"frm_payload\":\"BK0=\",\"decoded_payload\":{\"bytes\":[4,173],\"temperatur\":19.7},\"rx_metadata\":[{\"gateway_ids\":{\"gateway_id\":\"mikrotik-wap-lora8-kit-211024\",\"eui\":\"343632383F006F00\"},\"time\":\"2022-01-29T23:17:34.230808Z\",\"timestamp\":3152735292,\"rssi\":-68,\"channel_rssi\":-68,\"snr\":11,\"location\":{\"latitude\":48.820698154652284,\"longitude\":9.382012777368077,\"source\":\"SOURCE_REGISTRY\"},\"uplink_token\":\"CisKKQodbWlrcm90aWstd2FwLWxvcmE4LWtpdC0yMTEwMjQSCDQ2Mjg/AG8AELzYq98LGgwIjpbXjwYQmdueigEg4JSr7eClEioLCI6W148GEMCzh24=\",\"channel_index\":7}],\"settings\":{\"data_rate\":{\"lora\":{\"bandwidth\":125000,\"spreading_factor\":8}},\"coding_rate\":\"4/5\",\"frequency\":\"867900000\",\"timestamp\":3152735292,\"time\":\"2022-01-29T23:17:34.230808Z\"},\"received_at\":\"2022-01-29T23:17:34.291404810Z\",\"consumed_airtime\":\"0.092672s\",\"network_ids\":{\"net_id\":\"000013\",\"tenant_id\":\"ttn\",\"cluster_id\":\"ttn-eu1\"}}}', '2022-01-29 23:17:34');

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `daten`
--
ALTER TABLE `daten`
  ADD PRIMARY KEY (`id`),
  ADD KEY `zeit` (`zeit`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `daten`
--
ALTER TABLE `daten`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
COMMIT;
