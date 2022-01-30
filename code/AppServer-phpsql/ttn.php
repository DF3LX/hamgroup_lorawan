<?php
/********************************************************************
 * DARC-HamGroup LoRaWAN
 * 
 * http://www.p37.de/LoRaWAN
 * 
 *  Datenimport vom TTN-LoRaWAN-Server 
 *  
 *	29.01.2022	JM	vereinfacht und für Workshop angepasst 
 */

include( "i/include.php" );                                                     // Datenbankanbindung und -funktionen

$json_str = trim( file_get_contents('php://input') );                           // via TTN-Webhook übertragener JSON-String holen

if( strlen( $json_str ) > 0 ) {

    $funkdatensatz = json_decode( $json_str );                                  // JSON-String in ein Array konvertieren

    $cols = array (                                                             // Daten für SQL-Datenbank aufbereiten
        "json" => $json_str,                                                    // originaler JSON-String
        "devEUI" => $funkdatensatz->end_device_ids->dev_eui,                    // DevEUI
        "zeit" => date("Y-m-d H:i:s", time() )                                  // Zeitstempel
    );

    $db->insertDBRow( "daten", "id", $cols  );                                  // LoRaWAN-Aussendung in Datenbank packen
}   
?>
