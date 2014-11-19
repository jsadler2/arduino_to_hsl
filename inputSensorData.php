<?php

require_once 'AL_hidden_values.php';
require_once 'database_connection.php';

$info = $_GET["info"];
$data = $_GET["readings"];


$dateTimeArray= array();
$UTCArray = array ();
$MethodID = "1";
$QualityControlLevelID = "0";
$CensorCode = "nc";
$UTCOffset = "-7";



$parsedInfo = explode(",",$info);
$SourceID = $parsedInfo[0];
$SiteID= $parsedInfo[1];
$VariableID1 = $parsedInfo[2];
$VariableID2= $parsedInfo[3];


$PastTimeStamps = file_get_contents("DateRepository.txt");


$parsedData = explode(";",$data);

$sql7 ="INSERT INTO `datavalues`(`DataValue`, `LocalDateTime`, `UTCOffset`, `DateTimeUTC`, `SiteID`, `VariableID`, `CensorCode`, `MethodID`, `SourceID`, `QualityControlLevelID`) VALUES ";

for($i=0, $size=count($parsedData); $i<$size; ++$i){
	$parsedMoreData = explode(",",$parsedData[$i]); //parses the individual data records
	
	$mydate="20".$parsedMoreData[0].",".$parsedMoreData[1]; //formats the date so that it is 2014 instead of 14 and adds time value
	$timestamp=strtotime($mydate); 
	$mydate = date('Y-m-d H:i:s', $timestamp);
	array_push($dateTimeArray,$mydate);
	
	
	if (strpos($PastTimeStamps, $mydate)!== false){
		
		continue;
	}

	
	
	$ms = $UTCOffset * 3600;
	$utctimestamp = $timestamp - ($ms);
	$DateTimeUTC = date("Y-m-d H:i:s", $utctimestamp);
	
	$sql7.="('$parsedMoreData[2]', '$mydate', '$UTCOffset', '$DateTimeUTC', '$SiteID', '$VariableID1', '$CensorCode', '$MethodID', '$SourceID', '$QualityControlLevelID'), ";
	$sql7.="('$parsedMoreData[3]', '$mydate', '$UTCOffset', '$DateTimeUTC', '$SiteID', '$VariableID2', '$CensorCode', '$MethodID', '$SourceID', '$QualityControlLevelID'), ";
	
	
}
file_put_contents("DateRepository.txt", $dateTimeArray);


$sql7=substr($sql7,0,(strlen($sql7)-2));
echo ($sql7);
$result7 = @mysql_query($sql7,$connection)or die(mysql_error());
echo($result7);
echo ("tests");

require_once 'update_series_catalog_function.php';

update_series_catalog($SiteID, $VariableID1, $MethodID, $SourceID, $QualityControlLevelID);
update_series_catalog($SiteID, $VariableID2, $MethodID, $SourceID, $QualityControlLevelID);

	

	
	
?>