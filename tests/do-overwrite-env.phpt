--TEST--
Overwrite the existing env vars if dotenv.overwrite is On
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file = {PWD}/overwrite.env
dotenv.overwrite = On
variables_order = EGPCS
--ENV--
SOME_WEIRD_PRESET_ENV_VARIABLE=preset
--FILE--
<?php
echo $_ENV['SOME_WEIRD_PRESET_ENV_VARIABLE'], PHP_EOL;
--EXPECT--
overwritten
