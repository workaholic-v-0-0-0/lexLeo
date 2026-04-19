LexLeo
======

LexLeo is the LexLeo language command-line application.

Usage
-----

Run:

    lexleo

Configuration
-------------

System configuration file:

    /etc/lexleo/lexleo.conf

Current supported option:

    log_path=

If log_path is set, the specified path is used.
If empty or omitted, the application uses its default log file path.

Logs
----

The log file path depends on the configured value of:

    log_path

If no explicit path is configured, the default log file path is used:

    $HOME/.local/state/lexleo/lexleo.log

When the log file is created, the application writes a probe message to it.

Support
-------

Project repository:
https://github.com/workaholic-v-0-0-0/lexLeo

License
-------

GPL-3.0-or-later
