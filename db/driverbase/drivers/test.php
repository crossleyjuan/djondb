<?php

include("djonwrapper.php");

$c = DjondbConnectionManager::getConnection("localhost");
$c->open();

$json = "{ name: 'Peter', lastName: 'Parker', occupations: [ { company: 'Daily Bugle', position: 'Photographer'}, { position: 'Superhero' } ], nicknames: [{ name: 'Spiderman', main: 1}, {'name': 'Spìdey'}] }";

$c->insert('phpdb', 'superheroes', $json);
echo '<p>Inserted</p>';

$j['name'] = "Peter";
$j['lastName'] = "Parker";
$t = json_encode($j);
echo "new t: $t";
$c->insert('phpdb', 'a', $t);

echo '<p>Finding</p>';
$res = $c->find('phpdb', 'superheroes', '$"name" == "Peter"');

echo $res->toChar()."\n";

echo "nickname: ";

$res = $c->find('phpdb', 'superheroes', '*', '$"lastName" == "Parker"');

echo $res->toChar()."\n";

$res = $c->find('phpdb', 'superheroes', '*', '$"name" == "Peter"');
				
echo "With objects";

$res = json_decode($res->toChar());

foreach ($res as $obj) {
	//var_dump($obj);
	echo 'Name: '.$obj->{'name'};
}

DjondbConnectionManager::releaseConnection($c);

?>
