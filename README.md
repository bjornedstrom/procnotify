libprocnotify
=============

Note: This is in development and does not have a proper build system or final API.

Library for being notified on Linux process creation/destruction.

See `src/procnotify.h` for preliminary API documentation.

See `example.c` for an example how to use it.

Rationale
---------

Linux does not have a good non-intrusive way to get notifications for
process creation. One solution that many people seem to think of is to
use inotify on `/proc`, but that does not work.

This project solves the problem the easy way by just polling /proc in
a background thread.
