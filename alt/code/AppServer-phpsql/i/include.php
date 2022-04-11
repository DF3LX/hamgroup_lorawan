<?
/*******************************************************************************
 * Ing.-Büro Jürgen Mayer, www.jmayer.de / www.feuerwehr.de / www.dl8ma.de
 *
 * 02.12.2017   JM  auf PDO umgestellt 
 *
 */
include("config.php");

$db = new mysqlMapper( $dbZugangsdaten );

/**
 *	Wrapper-Klasse für MySQL
 *
 *	@date 	02.12.2017
 * 	@author	jmayer
 */
class mysqlMapper
{
    private $pdo;

    public function __construct( $dbZugangsdaten )
    {
        try {
            $this->pdo = new PDO("mysql:host=" . $dbZugangsdaten[ "host" ] . ";dbname=" . $dbZugangsdaten[ "name" ] . ";charset=utf8", $dbZugangsdaten[ "user" ], $dbZugangsdaten[ "pass" ], array(PDO::ATTR_PERSISTENT => true) );
        } catch(PDOException $e) {
            echo $e->getMessage();
        }
    }

    public function query( $query ) {
        $sql = $this->pdo->prepare( $query );
        if( $sql->execute() ) {
            return( $sql );
        } else {
            echo "Datenbankfehler :-(<br />";
        }
    }

    /**
     *	Datensatz neu anlegen
     *
     *	@date 	14.04.2004
     * 	@author	jmayer
     *
     *  02.12.2017  JM  in MySQL-Wrapper-Klasse übernommen
     *
     *	@param $tabname		Tabellenname
     *	@param $idname		Feldname des ID-Felds
     *	@param $cols		Array mit Auflistung der Felder und Werte die ge�ndert werden
     */
    public function insertDBRow($tabname, $idname, $cols ) {

        $query = "insert into " . $tabname . " (";
        $flag = 0;
        foreach ($cols as $k => $v) {
            if( $flag )
                $query = $query . ", ";
            $query = $query . $k;
            $flag = 1;
        }
        $query = $query . " ) values ( ";
        $flag = 0;
        foreach ($cols as $k => $v) {
            if( $flag )
                $query = $query . ", ";
            if (is_string ($v))
                $v = $this->toDBString($v);
            if ($v == null || !isset($v))
                $v = "''";
            $query = $query . $v;
            $flag = 1;
        }
        $query = $query . " )";
        $sql = $this->pdo->prepare( $query );
        if( !$sql->execute()) {
            echo "Datenbankfehler :-(<br />";

            echo "<pre>";
            print_r( $sql );
            echo "</pre>";

        }
        return( $this->pdo->lastInsertId() );
    }

    /**
     *	Datensatz updaten
     *
     *	@date 	14.04.2004
     * 	@author	jmayer
     *
     *  02.12.2017  JM  in MySQL-Wrapper-Klasse übernommen
     *
     *	@param $tabname		Tabellenname
     *	@param $idname		Feldname des ID-Felds
     *	@param $idval		ID des zu �ndernden Datensatzes
     *	@param $cols		Array mit Auflistung der Felder und Werte die ge�ndert werden
     *	@param $changed		Feldname des Zeitstempelfelds
     *	@param $changedby	Feldname des Feldes in der die Userid des �ndernden Users gespeichert wird
     */
    function updateDBRow ($tabname, $idname, $idval, $cols ) {
        $query = "update " . $tabname . " set ";
        $flag = 0;
        foreach ($cols as $k => $v) {
            if( $flag )
                $query = $query . ", ";
            if( is_string( $v ))
                $v = $this->toDBString($v);
            if ($v == null || !isset($v))
                $v = "''";
            $query = $query . $k . "=" . $v;
            $flag = 1;
        }
        $query = $query . " where " . $idname . "=" . $idval;
        $sql = $this->pdo->prepare( $query );
        if( !$sql->execute()) {
            echo "Datenbankfehler :-(<br />";
        }
    }


    /**
     * wandelt eine Zeichenkette in einen Datenbankzeichenkette um.
     * Die Zeichenkette wird dabei gequotet.
     * Ist die Zeichenkette gleich 'null' oder null, dann wird nicht gequotet.
     * @param $str Zeichenkette
     * @return gequotete Zeichenkette
     */
    function toDBString ($str) {
        if ($str == null || !isset($str) || $str == 'null')
            return "null";
        return "'" . $this->quoteDBString($str) . "'";
    }

    /**
     *	bereitet den übergebenen String für das Eintragen in die Datenbank mittels SQL-Anweisung vor.
     *  Dabei werden Hochkommas in der Zeichenkette entsprechend gequotet bzw. mit Escape-Zeichen versehen.
     *
     *	@param $str		Zeichenkette, die gequotet bzw. escaped werden soll.
     *	@return			Die übergebene und für eine SQL-Anweisung konvertierte Zeichenkette
     */
    function quoteDBString ($str) {
        return preg_replace ("/'/m", "''", $str);
    }

}


function datum( $datum ) {

    $data=explode("-",substr( $datum, 0, 10 ));
    if( strftime( "%Y", time() ) == $data[0] )
        $dat=$data[2].".".$data[1].".";
    else
        $dat=$data[2].".".$data[1].".".$data[0];
    $dat=$data[2].".".$data[1].".".$data[0];

    $zeit = substr( $datum, 11, 8 );
    $lcstring = $dat . " " . $zeit;

    return( $lcstring );
}


?>
