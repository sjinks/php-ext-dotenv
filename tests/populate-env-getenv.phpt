--TEST--
Check $_ENV and getenv() match
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/sample.env
dotenv.populate_env = On
--FILE--
<?php
assert($_ENV['FOO'] === getenv('FOO'));
assert($_ENV['SOMETHING'] === getenv('SOMETHING'));
assert($_ENV['SOMETHING_ELSE'] === getenv('SOMETHING_ELSE'));
assert($_ENV['VALUE'] === getenv('VALUE'));
assert($_ENV['EMPTY'] === getenv('EMPTY'));
assert($_ENV['NULLVAL'] === getenv('NULLVAL'));
--EXPECT--
