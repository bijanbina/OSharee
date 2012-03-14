
#Ensure that lates version of autoconf avalible
AC_PREREQ([2.63])

#create info about project
AC_INIT([OSharee],[1.0], , OSharee)

#foreign use to break the gnu strict
AM_INIT_AUTOMAKE([foreign dist-bzip2])

#create information for package
AC_CONFIG_HEADERS([config.h])

# Use it to link library
AC_PROG_LIBTOOL

# check for gtk-doc
#GTK_DOC_CHECK([1.14],[--flavour no-tmpl])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

PKG_CHECK_MODULES([LibNotify], [libnotify  >= 0.7.0])
#force to less verbose build output
AM_SILENT_RULES([yes])

AC_CONFIG_FILES([
	Makefile
	Source/Makefile
  Resources/Makefile
])

AC_OUTPUT

