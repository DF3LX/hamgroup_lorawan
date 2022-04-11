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
 * Tabelle der letzten Aussendungen
 * 
 *  Parameter:  devEUI          Device-EUI des ausgewählten Nodes
 *              label           Messwert der in der Tabelle mit anzeigt wird ( optional )
 * 
 *  Beispiel:   www.meinedomain.de/darc/liste.php?devEUI=A9D985734D88B560&label=temperatur
 * 
 *	29.01.2022	JM	vereinfacht und für Workshop angepasst 
 */

error_reporting(E_ALL ^ E_NOTICE);
ini_set('display_errors', 1);

include("i/include.php");

$devEUI = substr( $_REQUEST[ "devEUI" ], 0, 40 );                                                               // DevEUI 
$label = substr( $_REQUEST[ "label" ], 0, 40 );                                                                 // Label eines Messwertes

echo "<table cellpadding=\"3\" border=\"1\">";
echo "<tr><th></th><th><font size=1>Gateways</th><th><font size=1>RSSI</th><th><font size=1>" . $label . "</th></tr>";

$query = "select * from daten where devEUI  = \"" . $devEUI . "\" order by id desc limit 0,50";                 // letzte 50 Aussendungen eines Nodes holen
foreach ($db->query( $query ) as $row) {
    $funkdatensatz = json_decode( $row[ "json" ] );                                                             // JSON-String mit Daten und Metadaten
    $gatewayAnzahl = sizeof( $funkdatensatz->uplink_message->rx_metadata );                                     // Anzahl der Gateways ermitteln

    $rssi = 0;
    for( $i = 0; $i < $gatewayAnzahl; $i ++ ) {                                                                 // alle an der Übertragung beteiligten Gateways
        $gateway = $funkdatensatz->uplink_message->rx_metadata[ $i ];
        if( $rssi == 0 )                                                                                        // RSSI der besten Übertragung ermitteln
            $rssi = $gateway->rssi;
        if( $rssi < $gateway->rssi )
            $rssi = $gateway->rssi;
    }

    echo "<tr><td>";                                                                                            // Tabelle aufbauen und ausgeben
    echo "<a href=\"anzeige.php?id=" . $row[ "id" ] . "\" target=\"_blank\">";
    echo datum( $row[ "zeit" ] );
    echo "</a>";

    echo "</td><td>";
    echo $gatewayAnzahl;
    echo "</td><td>";
    echo $rssi;
    echo "</td><td>";

    $payloadArray = cvf_convert_object_to_array( $funkdatensatz->uplink_message->decoded_payload );             // dekodierte Payload in ein Array packen
    echo $payloadArray[ $label ];                                                                               // ausgewählter Wert ausgeben
    echo "</td></tr>";
}
echo "</table>";


/********************************************************
 *  konvertiert ein Objekt in ein Array 
 */
function cvf_convert_object_to_array($data) {
        if (is_object($data)) {
            $data = get_object_vars($data);
        }
    
        if (is_array($data)) {
            return array_map(__FUNCTION__, $data);
        }
        else {
            return $data;
        }
    }

?>
</body></html>
