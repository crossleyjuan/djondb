<?php

function microtimefloat() {
	list($usec,$sec)=explode(" ", microtime());
	return ((float)$usec + (float)$sec);
}

function test() {
	$c = DjondbConnectionManager::getConnection("localhost");
	$c->open();

	echo '<p>Finding</p>';
	$res = $c->find('astra', 'astralog', '$"time" > 100000000');

	echo "find returned";

	echo "result: \n".$res->toChar()."\n";

	DjondbConnectionManager::releaseConnection($c);
}

include("djonwrapper.php");

$time_start = microtimefloat();

test();

$time_end = microtimefloat();
$time=$time_end-$time_start;


echo "did it in $time seconds";

?>
