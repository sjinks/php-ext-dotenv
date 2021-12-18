--TEST--
Test module info
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
phpinfo(INFO_MODULES);
?>
--EXPECTREGEX--
.*dotenv extension\s+=>\s+enabled.*
