<?php
include("djonwrapper.php");

/**
*  * Simple function to replicate PHP 5 behaviour
*   */
function microtime_float()
{
	    list($usec, $sec) = explode(" ", microtime());
		     return ((float)$usec + (float)$sec);
}


$c = DjondbConnectionManager::getConnection("localhost");
$c->open();

echo "doing find over astra: ";

$time_start = microtime_float();

$res = $c->find('astra', 'astralog', '$"time" > 100000');

for ($x = 0; $x < $res->size(); $x++) {
	echo $res->get($x)->toChar();
}

DjondbConnectionManager::releaseConnection($c);

$time_end = microtime_float();
$time = $time_end - $time_start;

echo "Executed in $time secords\n";
?>
