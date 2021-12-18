--TEST--
Load ini file (populate $_ENV)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
dotenv.populate_env = On
--FILE--
<?php
var_export($_ENV['FOO']);
echo PHP_EOL;
var_export($_ENV['SOMETHING']);
echo PHP_EOL;
var_export($_ENV['SOMETHING_ELSE']);
echo PHP_EOL;
var_export($_ENV['VALUE']);
echo PHP_EOL;
var_export($_ENV['EMPTY']);
echo PHP_EOL;
var_export($_ENV['NULLVAL']);
echo PHP_EOL;
--EXPECT--
'bar'
'on'
'off'
'123'
''
'null'
