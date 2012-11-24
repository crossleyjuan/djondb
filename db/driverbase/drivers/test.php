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

for ($x = 0; $x < $res->size(); $x++) {
	echo $res->get($x)->toChar();
}

echo "nickname: ";

$res = $c->find('phpdb', 'superheroes', '$"nicknames.name" == "Spiderman"');

for ($x = 0; $x < $res->size(); $x++) {
	echo $res->get($x)->toChar();
}

$res = $c->find('phpdb', 'superheroes', '$"lastName" == "Parker"');

for ($x = 0; $x < $res->size(); $x++) {
	echo $res->get($x)->toChar();
}

$res = $c->find('phpdb', 'superheroes', '$"name" == "Peter"');
				
echo "With objects";

for ($x = 0; $x < $res->size(); $x++) {
	$obj = json_decode($res->get($x)->toChar());
	//var_dump($obj);
	echo 'Name: '.$obj->{'name'};
}

DjondbConnectionManager::releaseConnection($c);

?>
