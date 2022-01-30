<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=UTF-8">
<meta http-equiv="refresh" content="30">
<title>HamGroup LoRaWAN</title>
<style type="text/css">
	body {font-size:90%;
	font-family:"Verdana", Times, serif;
}
</style>
</head>
<body>

<?php
/********************************************************************
 * DARC-HamGroup LoRaWAN
 *
 * http://www.p37.de/LoRaWAN
 *  
 * Anzeige der Daten bzw. Meta-Daten der einer Aussendungen
 * 
 *  Parameter:  devEUI          Device-EUI des ausgewählten Nodes
 * 
 *  Beispiel:   www.meinedomain.de/darc/anzeige.php?devEUI=A9D985734D88B560
 * 
 *	29.01.2022	JM	vereinfacht und für Workshop angepasst 
 */

error_reporting(E_ALL ^ E_NOTICE);
ini_set('display_errors', 1);

include( "i/include.php" );

$devEUI = substr( $_REQUEST[ "devEUI" ], 0, 40 );															// DevEUI des Nodes der angezeigt werden soll

if( isset( $_REQUEST[ "id" ] ) ) {																			// für einen bestimmten Datensatz
	$id = (int) $_REQUEST[ "id" ];
}
else
	$id = 0;


if( $id == 0 )
	$query = "select * from daten where devEUI  = \"" . $devEUI . "\" order by id desc limit 0,1"; 			// aktuellste Aussendung eines bestimmter Node
else
	$query = "select * from daten where id  = " . $id . " order by id desc limit 0,1"; 						// eine bestimmte Ausendung

foreach ($db->query( $query ) as $row) {
}

$funkdatensatz = json_decode( $row[ "json" ] );																// kompletter Datensatz mit Daten und Metadaten

/* ab hier Ausgabe */

echo datum( $row[ "zeit" ] );
echo "<hr>";
echo "<b>Device-EUI:</b><p>";																				// DevEUI
echo strtoupper( $funkdatensatz->end_device_ids->device_id );

echo "<p>";
$payload = $funkdatensatz->uplink_message->decoded_payload; 
echo "<hr>";
echo "<b>dekodierte Payload:</b>";
echo "<pre>";
print_r( $payload );																						// entschlüsselte Payload
echo "<pre>";
echo "<hr>";

echo "<b>Datenrate:</b>";																					// Metadaten der Aussendung											
$qrg = 		$funkdatensatz->uplink_message->settings->frequency;
$sf = 		"SF " . $funkdatensatz->uplink_message->settings->data_rate->lora->spreading_factor;
$bandbreite = 	$funkdatensatz->uplink_message->settings->data_rate->lora->bandwidth;
$sendezeit =	$funkdatensatz->uplink_message->consumed_airtime;
$bandbreite = $bandbreite / 1000;
$qrg = $qrg / 1000000;

echo "<table cellpadding=\"3\" border=\"1\">";																// Tabelle dazu
echo "<tr><td>";
echo "Speading Faktor";
echo "</td><td>";
echo $sf;
echo "</td></tr>";

echo "<tr><td>";
echo "Brandbreite";
echo "</td><td>";
echo $bandbreite . " kHz";
echo "</td></tr>";

echo "<tr><td>";
echo "QRG";
echo "</td><td>";
echo $qrg . " MHz";
echo "</td></tr>";

echo "<tr><td>";
echo "Sendezeit: ";
echo "</td><td>";
echo $sendezeit;
echo "</td></tr>";

echo "</table>";

echo "<hr>";

echo "<b>Gateways:</b><p>";																					// Liste der genutzten Gateways 

echo "<table cellpadding=\"3\" border=\"1\">";																// Tabelle dazu
echo "<tr><th>Gateway-Id</th><th>RSSI</th><th>SNR</th><th colspan=\"2\">Locator</th></tr>";

$gatewayAnzahl = sizeof( $funkdatensatz->uplink_message->rx_metadata );										// Anzahl der Gateways
for( $i = 0; $i < $gatewayAnzahl; $i ++ ) {
	$gateway = $funkdatensatz->uplink_message->rx_metadata[ $i ];
	echo "<tr><td>";
	echo $gateway->gateway_ids->gateway_id;
	echo "</td><td>";
	echo $gateway->rssi;
        echo "</td><td>";
        echo $gateway->snr;
        echo "</td><td>";
        echo $gateway->location->latitude . "</td><td> " . $gateway->location->longitude;					
        echo "</td><td>";
	if( strlen( $gateway->location->latitude ) > 0 )  														// falls verfügbar mit Link zum GW-QTH
		echo "<a href=\"https://www.google.com/maps?q=" . $gateway->location->latitude . "," . $gateway->location->longitude . "\" target=\"_blank\">X</a>";
	echo "</td></tr>";
}
echo "</table>";
echo "<hr>";
echo "<p>";
echo "<hr>";
echo "<b>kompletter Datensatz vom LoRaWAN-Server</b>";														// Roh-Datensatz vom LoRaWAN-Server
echo "<pre>";
print_r( $funkdatensatz );
echo "<pre>";
?>
</body></html>
