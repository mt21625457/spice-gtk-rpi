spice-gtk
=========
这是树梅派4B的特别优化版本，后续将跟官方版本同步更新，测试稳定后，会将测试结果和代码全部上传。 

安装编译教程也会同步更新。

不要在非树梅派4B下使用此版本，其他客户端请使用官方的版本。

A Gtk client and libraries for SPICE remote desktop servers.

Please report bugs at: spice-devel@lists.freedesktop.org

Project content
---------------

* **libspice-client-glib-2.0**

    Provides glib objects for spice protocol decoding and surface rendering.
    * SpiceSession (see spice-session.h).
    * SpiceChannel (see spice-channel.h).
    * SpiceAudio (see spice-audio.h).
    * Various Spice<Type>Channel (see channel-<type>.h).

* **libspice-client-gtk-3.0**

    provides gtk widget to show spice display and accept user input.
    * SpiceDisplay (see spice-widget.h)

* **spicy**

   a gtk test client. The recommended client for end user is
   [virt-viewer]

* **spicy-screenshot**

   Command line tool, connects to spice server and writes out a
   screen shot.

* **spicy-stats**

   Command line tool, connects to spice server and writes out a
   summary of connection details, amount of bytes transferred...

* **SpiceClientGlib** and **SpiceClientGtk** GObject-introspection modules.

[virt-viewer]: https://pagure.io/virt-viewer

Build dependencies:
------------------

* On Fedora use:

>>>
    dnf builddep spice-gtk
>>>

* or install:

>>>
    meson ninja gtk3-devel spice-protocol openssl-devel pulseaudio-libs-devel pixman-devel
    gobject-introspection-devel libjpeg-turbo-devel zlib-devel cyrus-sasl-devel gtk-doc
    gettext-devel vala vala-tools python3 python3-pyparsing
>>>

The GStreamer backend needs:

>>>
    gstreamer1-devel gstreamer1-plugins-base-devel gstreamer1-plugins-good gstreamer1-plugins-bad-free
>>>
