# php-ext-dotenv

A PHP extension that sets environment variables from a file on request startup.

## Installation

```bash
phpize
./configure
make
sudo make install
```

## Configuration

The only configuration option is `dotenv.file`. It expects the name of the environment file. The setting is a [per-directory](https://www.php.net/manual/en/configuration.changes.modes.php) one.

Example:
```ini
dotenv.file = /path/to/my/.env
```

## How It Works

The extension reads the file specified in the `dotenv.file` setting during the request startup, [parses that file as an INI-file](https://www.php.net/manual/en/function.parse-ini-file.php), and popukates the `$_ENV` superglobal with the keys and values read from the file.
