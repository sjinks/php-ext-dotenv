--TEST--
dotenv.cli.file (CLI, use CWD)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--INI--
dotenv.file={PWD}/generic.env
dotenv.cli.file=cli.env
dotenv.cli.use_script_dir=Off
--FILE--
<?php
var_export(getenv('SPECIAL_ENV_VAR'));
--EXPECT--
false
