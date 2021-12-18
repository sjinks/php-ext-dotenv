--TEST--
Load ini file (do not populate $_ENV)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
dotenv.populate_env = Off
--FILE--
<?php
assert(!isset($_ENV['FOO']));
assert(!isset($_ENV['SOMETHING']));
assert(!isset($_ENV['SOMETHING_ELSE']));
assert(!isset($_ENV['VALUE']));
assert(!isset($_ENV['EMPTY']));
assert(!isset($_ENV['NULLVAL']));

echo getenv('FOO'), PHP_EOL;
echo getenv('SOMETHING'), PHP_EOL;
echo getenv('SOMETHING_ELSE'), PHP_EOL;
echo getenv('VALUE'), PHP_EOL;
echo getenv('EMPTY'), PHP_EOL;
echo getenv('NULLVAL'), PHP_EOL;
--EXPECT--
bar
on
off
123

null
