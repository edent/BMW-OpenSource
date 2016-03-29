Name:		%{_stm_pkg_prefix}-target-qt-embedded
Version:	4.6.0
Release:	27
License:	LGPL
Source:		ftp://ftp.trolltech.com/qt/source/qt-everywhere-opensource-src-%{version}.tar.gz
URL:		http://www.trolltech.com
Buildroot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-%{_stm_target_arch}-XXXXXX)
Prefix:		%{_stm_cross_target_dir}
#BuildRequires:	%{_stm_pkg_prefix}-host-rpmconfig 
%if_target_cpu st200
BuildRequires:  %{_stm_pkg_prefix}-%{_stm_target_arch}-st200libstdc++
%endif
%if_target_arch sh4 sh4_uclibc
BuildRequires:  %{_stm_pkg_prefix}-%{_stm_target_arch}-libstdc++-dev
%endif

Patch0: qt-embedded-%{version}-st200.patch
Patch1: qt-embedded-%{version}-sh4.patch
Patch2: qt-embedded-%{version}-sh4_uclibc.patch
Patch3: qt-embedded-%{version}-misaligned.patch
Patch4: qt-embedded-%{version}-arm.patch
Patch5: qt-embedded-%{version}-cortex.patch
Patch6: qt-embedded-%{version}-mediaplayer_loop.patch
Patch7: qt-embedded-%{version}-st200-cflags-debug.patch
Patch8: qt-embedded-%{version}-mmap.patch
Patch9: qt-embedded-%{version}-no-messagebox.patch
Patch10: qt-embedded-%{version}-enable-fast-malloc-match-validation.patch

# This is a temporary bodge until the kernel is sorted out
Patch200: qt-embedded-%{version}-st200-NR_inotify.patch

%define _pkgname %{_stm_pkg_prefix}-%{_stm_target_arch}-qt-embedded
%define _docdir		%{_stm_cross_target_dir}%{_stm_target_doc_dir}

#
#  Source Package
#

Summary:	The Qt GUI toolkit for embedded systems.
Group:		System Environment/Libraries

%description
This is the source RPM for the qt-embedded binary package.

Qt is a GUI software toolkit which simplifies the task of writing and
maintaining GUI (Graphical User Interface) applications.

#
#  Library Package
#

%package -n %{_pkgname}
Summary:	The shared library for the Qt-embedded GUI toolkit.
Group:		System Environment/Libraries

%description -n %{_pkgname}
Qt-embedded is a GUI software toolkit which simplifies the task of
writing and maintaining GUI (Graphical User Interface) applications
for embedded systems using the Linux framebuffer.

Qt is written in C++ and is fully object-oriented.

This package contains the shared libraries needed to run Qt applications,
as well as the README files for Qt.

#
#  Dev Package
#

%package -n %{_pkgname}-dev
Summary:	Development files for the Qt-embedded GUI toolkit.
Group:		Development/Libraries

%description -n %{_pkgname}-dev
The qt-embedded-dev package contains the files necessary to develop
applications using the Qt GUI toolkit: the header files, the Qt meta
object compiler and the static libraries.

#
#  Examples Package
#

%package -n %{_pkgname}-examples
Summary:	Examples for the Qt-embedded GUI toolkit.
Group:		Documentation/Development

%description -n %{_pkgname}-examples
The qt-embedded-examples package contains example programs 
and tutorials to help with beginning Qt programming.

#
#  Demo Package
#

%package -n %{_pkgname}-demos
Summary:	Demos for the Qt-embedded GUI toolkit.
Group:		Documentation/Development

%description -n %{_pkgname}-demos
The qt-embedded-demos package contains various pre built 
binaries and corresponding source showing some of QT's capabilities.
These programs are useful as a test that Qt is working correctly.

#
#  Doc Package
#
%package -n %{_pkgname}-doc
Summary:	Documentation for the Qt-embedded GUI toolkit.
Group:		Documentation/Development

%description -n %{_pkgname}-doc
The qt-embedded-doc package contains the documentation necessary to
develop applications using the Qt GUI toolkit. The documentation is 
provided in HTML format. 

See http://www.trolltech.com for more information about Qt.

%prep
%setup -q -n qt-everywhere-opensource-src-%{version}

%patch0 -p1
%patch1 -p1
%patch2 -p1
%patch3 -p1
%patch4 -p1
%patch5 -p1
%patch6 -p1
%patch7 -p1
%patch8 -p1
# Use the following patch to disable qt browser javascript timeouts, for benchmarking
# or debugging.
# %patch9 -p1
# Use the following patch to enable debugging in the Qt malloc implementation
# %patch10 -p1

%patch200 -p1

%build
%target_setup

%define _qt_data_dir "%{_stm_target_data_dir}/qt-%{version}"

# QT knows how to cross compile, it thinks CC is the host  
# compiler so you end up with a qmake that cannot run. 
# It would be nice to use the qmake we built in the cross
# build, but this does not seem to be that easy to accomplish

unset CC AS LD AR RANLIB CXX NM CFLAGS CXXFLAGS

# We set -no-rpath as the QT libs are in a sensible place
# We also don't build QT3 support, don't see much point in this.

./configure -confirm-license \
	    -verbose -verbose \
	    -prefix "%{_stm_target_prefix}" \
	    -bindir "%{_stm_target_bin_dir}" \
	    -libdir "%{_stm_target_lib_dir}" \
	    -docdir "%{_stm_target_doc_dir}/qt-%{version}" \
	    -headerdir "%{_stm_target_include_dir}" \
	    -plugindir "%{_stm_target_lib_dir}/qt-%{version}" \
	    -datadir %{_qt_data_dir} \
	    -translationdir %{_qt_data_dir}/translations \
	    -sysconfdir "%{_stm_target_sysconf_dir}/qt-%{version}" \
	    -examplesdir "%{_qt_data_dir}/examples" \
	    -demosdir "%{_qt_data_dir}/demos" \
	    -plugin-gfx-directfb \
	    -no-qt3support \
            -no-exceptions \
	    -no-rpath \
            -stl \
	    -reduce-relocations \
            -force-pkg-config \
            -system-sqlite \
	    -system-zlib \
	    -system-libpng \
	    -system-libmng \
	    -system-libjpeg \
	    -no-mmx -no-3dnow -no-sse -no-sse2 \
            -no-cups \
            -little-endian \
            -host-little-endian \
            -depths 8,15,16,24,32  \
	    -opensource \
	    -embedded %{_stm_target_arch} \
	    -arch %{_stm_target_arch}

%make 

%install
%target_setup

unset CC AS LD AR RANLIB CXX CFLAGS CXXFLAGS

%define _install_prefix	%{buildroot}%{_stm_cross_target_dir}

%make INSTALL_ROOT="%{_install_prefix}" install

# Fix up the pkgconfig files
%target_install_fixup

# The .prl files used by qmake are only suitable for use by the target, they
# have -L/usr/lib liberally sprinkled all over them. What we could do is 
# replace them with -L$$[QT_INSTALL_LIBS] which will give the correct value 
# regardless of if they are used in cross or target enviroments. However, since 
# we install the QT libs in the default of /usr/lib anyway it is cleaner to 
# simply remove any mention of them and it will still work fine.
# We also delete the QMAKE_PRL_BUILD_DIR, as it will refer to the build machine
sed -i -e 's,-L/usr/lib,,g' -e '/^QMAKE_PRL_BUILD_DIR/d' %{_install_prefix}%{_stm_target_lib_dir}/*.prl

# The pkg files are not "usual" as they are generated by qmake.
# Although our libtool will handle the -L/usr/lib correctly, again
# we delete them here to make things cleaner
sed -i 's,-L/usr/lib,,g' %{_install_prefix}%{_stm_target_lib_dir}/*.la

# These are the cross versions, until we can figure out how to 
# build target versions of these tools.
rm -f %{_install_prefix}%{_stm_target_bin_dir}/{moc,qmake,rcc,uic}

%files -n %{_pkgname}
%defattr(-,root,root)

# Libraries
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.so.4                      
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.so.4.6                    
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.so.%{version}                  
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.so.4                      
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.so.4.6                    
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.so.%{version}                  
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.so.4                       
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.so.4.6                     
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.so.%{version}                   
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.so.4                       
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.so.4.6                     
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.so.%{version}                   
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.so.4                   
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.so.4.6                 
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.so.%{version}               
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.so.4.6
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.so.%{version}
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.so.4
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.so.4.3
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.so.4.3.1

# Dynamic plugins
# If the system libs are used, should these appear, eg jpeg
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/accessible/libqtaccessiblewidgets.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/iconengines/libqsvgicon.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqgif.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqico.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqjpeg.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqmng.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqsvg.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/imageformats/libqtiff.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/phonon_backend/libphonon_gstreamer.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/script/libqtscriptdbus.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/sqldrivers/libqsqlite.so
# Directfb plugin 
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/gfxdrivers/libqdirectfbscreen.so

# Translations
%{_stm_cross_target_dir}%{_stm_target_sharedstate_dir}/qt-%{version}/translations

# The fonts, don't like the fact they end up here, but there is not much I can do about that.
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/fonts

# If you want QT3 compatibility, these are the files that are produced
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.so.4                  
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.so.4.6                
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.so.%{version}              
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/qt-%{version}/accessible/libqtaccessiblecompatwidgets.so


# Licenses etc
%doc README INSTALL 
#%doc GPL_EXCEPTION_ADDENDUM.TXT GPL_EXCEPTION.TXT LICENSE.GPL2 
%doc LGPL_EXCEPTION.txt LICENSE.LGPL 
#%doc LICENSE.GPL3 OPENSOURCE-NOTICE.TXT
%doc LICENSE.GPL3

%files -n %{_pkgname}-dev
%defattr(-,root,root)
# The header files (all 1280 of them!)
%{_stm_cross_target_dir}%{_stm_target_include_dir}/Qt
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtCore
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtDBus
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtGui
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtMultimedia
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtNetwork
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtScript
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtScriptTools
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtSql
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtSvg
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtTest
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtXml
%{_stm_cross_target_dir}%{_stm_target_include_dir}/QtWebKit
%{_stm_cross_target_dir}%{_stm_target_include_dir}/phonon

# qmake spec files (nothing to do with RPM ones)
%{_stm_cross_target_dir}%{_stm_target_sharedstate_dir}/qt-%{version}/mkspecs

# Pkgconfig files
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/phonon.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtCore.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtDBus.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtGui.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtMultimedia.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtNetwork.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtScript.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtScriptTools.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtSql.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtSvg.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtTest.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtWebKit.pc
%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/QtXml.pc

# Libraries
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libphonon.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtCore.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtDBus.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtGui.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtMultimedia.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtNetwork.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScript.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtScriptTools.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSql.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtSvg.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtTest.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtWebKit.so
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.la
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.prl
%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQtXml.so

# QT3 support files
#%{_stm_cross_target_dir}%{_stm_target_include_dir}/Qt3Support
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.la
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.prl
#%{_stm_cross_target_dir}%{_stm_target_lib_dir}/libQt3Support.so
#%{_stm_cross_target_dir}%{_stm_target_pkgconfig_dir}/Qt3Support.pc

%files -n %{_pkgname}-examples
%defattr(-,root,root)
%{_stm_cross_target_dir}%{_stm_target_sharedstate_dir}/qt-%{version}/examples

%files -n %{_pkgname}-demos
%defattr(-,root,root)
%{_stm_cross_target_dir}%{_stm_target_sharedstate_dir}/qt-%{version}/demos

%files -n %{_pkgname}-doc
%defattr(-,root,root)
%doc doc/html
%doc README INSTALL 
%doc LICENSE.LGPL LGPL_EXCEPTION.txt
%doc LICENSE.GPL3

%clean
rm -rf %{buildroot}

%changelog
* Wed Mar 31 2010 Stuart Menefy <stuart.menefy@st.com> 27
- [Spec] Bump the release number for 2.4 product release.
- [Spec] Update BuildRoot to use %%(mktemp ...) to guarantee a unique name.

* Fri Mar  5 2010 Stephen Clarke <stephen.clarke@st.com> 26
- [Modify patch: qt-embedded-4.6.0-st200-cflags-debug.patch]  Remove use of
  -INLINE:all_inline.  Use -OPT:olimit=12000 to limit compilation VM usage.

* Fri Mar  5 2010 Stephen Clarke <stephen.clarke@st.com> 26
- [Spec] Configure with -no-exceptions.  Remove QtXml from delivered files, since
  this is not built when configured with -no-exceptions.
- [Add patch: qt-embedded-4.6.0-enable-fast-malloc-match-validation.patch] Enable
  TCMalloc internal validation checks (for debugging Qt only). This patch is
  listed since it may be useful to developers, but it is not applied.

* Thu Jan 21 2010 Stephen Clarke <stephen.clarke@st.com> 26
- [Add patch: qt-embedded-4.6.0-st200-cflags-debug.patch] Fix compiler flags for
  debug build on st200.
- [Add patch: qt-embedded-4.6.0-mmap.patch] Fix bug in TCMalloc implementation
  Ref: https://bugs.webkit.org/show_bug.cgi?id=33905
- [Add patch: qt-embedded-4.6.0-no-messagebox.patch] Disable javascript timeouts
  (for benchmarking and debugging only).

* Wed Jan 20 2010 Carmelo Amoroso <carmelo.amoroso@st.com> 25
- [Spec] Force rebuild for updated libasound.so not using versioned
  symbols on uClibc

* Thu Dec 17 2009 David McKay <david.mckay@st.com> 24
- [Update: 4.6.0 ] Upgrade to Qt version 4.6.0  
- [Spec] Add -reduce-relocations to cut down on PLT usage
- [Add patch: qt-embedded-4.6.0-st200.patch, qt-embedded-4.6.0-sh4.patch, qt-embedded-4.6.0-sh4_uclibc.patch]
- [Add patch: qt-embedded-4.6.0-misaligned.patch, qt-embedded-4.6.0-arm.patch, qt-embedded-4.6.0-cortex.patch]
- [Add patch: qt-embedded-4.6.0-st200-NR_inotify.patch, qt-embedded-4.6.0-mediaplayer_loop.patch]
- [Delete patch: qt-embedded-4.5.2-st200.patch, qt-embedded-4.5.2-sh4.patch, qt-embedded-4.5.2-sh4_uclibc.patch]
- [Delete patch: qt-embedded-4.5.2-misaligned.patch, qt-embedded-4.5.2-signbit.patch, qt-embedded-4.5.2-arm.patch]
- [Delete patch: qt-embedded-4.5.2-st200-NR_inotify.patch, qt-embedded-4.5.2-uclibc-isnan.patch, qt-embedded-4.5.2-cortex.patch]

* Tue Nov 24 2009 Chris Smith <chris.smith@st.com> 23
- [Add patch: qt-embedded-4.5.2-uclibc-isnan.patch]
  Fix build on uClibc by qualifying some calls to isnan() and
  isinf() with the std namespace.

* Mon Nov 03 2009 Melwyn Lobo <melwyn.lobo@st.com> 22
- [Add patch: qt-embedded-4.5.2-cortex.patch]
  Patch to allow Cortex compilation

* Wed Oct 08 2009 David McKay <david.mckay@st.com> 21
- [Add patch: qt-embedded-4.5.2-mediaplayer_loop.patch]
  Patch to allow mediaplayer to loop videos without pressing play
 
* Wed Sep 16 2009 Stephen Clarke <stephen.clarke@st.com> 20
- [Modify patch: qt-embedded-4.5.2-st200.patch] Set QT_NO_FPU for st231.

* Tue Sep 8 2009 David McKay <david.mckay@st.com> 19
- [Update: 4.5.2 ] Upgrade to Qt version 4.5.2  
- [Delete patch: qt-embedded-4.4.3-st200.patch, qt-embedded-4.4.3-sh4.patch] 
- [Delete patch: qt-embedded-4.4.3-sh4_uclibc.patch, qt-embedded-4.4.3-directfb.patch] 
- [Delete patch: qt-embedded-4.4.3-gcc3.3-canbuildwebkit.patch, qt-embedded-4.4.3-misaligned.patch]
- [Delete patch: qt-embedded-ca9.patch]
- [Add patch: qt-embedded-4.5.2-st200.patch, qt-embedded-4.5.2-sh4.patch, qt-embedded-4.5.2-sh4_uclibc.patch]
- [Add patch: qt-embedded-4.5.2-misaligned.patch, qt-embedded-4.5.2-signbit.patch, qt-embedded-4.5.2-arm.patch]
- [Add patch: qt-embedded-4.5.2-st200-NR_inotify.patch]

* Tue May 26 2009 David McKay <david.mckay@st.com> 18
- [Spec] Force rebuild for ST231 libpthread versioning problem

* Wed Feb 18 2009 Melwyn Lobo <melwyn.lobo@st.com> 16
- [Add patch : qt-embedded-linux-opensource-src-4.4.3-cortex.patch] Support for CA9
- [Spec] Changed _target_arch checks for arm and cortex to _target_cpu 

* Wed Feb 18 2009 David McKay <david.mckay@st.com> 16
- [Modify patch : qt-embedded-4.4.3-st200.patch] Add ST200 platform define
- [Modify patch : qt-embedded-4.4.3-sh4.patch]   Add SH4 platform define
- [Add patch: qt-embedded-4.4.3-misaligned.patch] 
  Removed misaligned accesses from WebKit

* Mon Jan 19 2009 Stephen Clarke <stephen.clarke@st.com> 15
- [Add patch: qt-embedded-4.4.3-gcc3.3-canbuildwebkit.patch] Enables build
  with gcc 3.3.3.
- [Spec] Install webkit for st200 also.

* Tue Dec  2 2008 David McKay <david.mckay@st.com> 14
- [Spec] Moved documentation from cross to target package
  to obsolete old Qt3 documentation correctly

* Fri Nov 28 2008 David McKay <david.mckay@st.com> 13
- [Update: 4.4.3] Upgrade to QT version 4.4.3
- [Add patch: qt-embedded-4.4.3-st200.patch] ST200 support for QT4
- [Add patch: qt-embedded-4.4.3-sh4.patch] SH4 support for QT4
- [Add patch: qt-embedded-4.4.3-sh4_uclibc.patch] SH4_uclibc support for QT4
- [Add patch: qt-embedded-4.4.3-directfb.patch] Enables use of directfb backend
- [Add patch: qt-embedded-4.4.3-st200-NR_inotify.patch] Temporary workaround
- [Delete patch: qt-embedded-free-3.1.2-sh4.patch]
- [Delete patch: qt-embedded-free-3.1.2-st231.patch]
- [Delete patch: qt-embedded-free-3.1.2-arm.patch] 
- [Delete patch: qt-embedded-free-3.1.2-sh4_uclibc.patch]

* Tue Oct  7 2008 Stephen Clarke <stephen.clarke@st.com> 12
- [Spec] Fix BuildRequires for st200

* Wed Mar 05 2008 David McKay <david.mckay@st.com> 11
- [Modify patch: qt-embedded-free-3.1.2-arm.patch] 
  Compiler now has the correct name by default

* Fri Aug 3 2007 Giuseppe Cavallaro <peppe.cavallaro@st.com> 10
- [Spec] Use if_target_arch conditional

* Wed Aug 1 2007 Giuseppe Cavallaro <peppe.cavallaro@st.com> 10
- [Spec] ported on uClibc build system.
- [Add patch: qt-embedded-free-3.1.2-sh4_uclibc.patch] qmake configuration 
  for sh4 uClibc.

* Wed Dec 13 2006  David McKay <david.mckay@st.com> 9
- ARM fixes

* Thu Nov 23 2006 Giuseppe Condorelli <giuseppe.condorelli@st.com>
- Fixed configure command line.
- Updated release number.

* Wed Nov 15 2006 Giuseppe Condorelli <giuseppe.condorelli@st.com>
- Modified configure call to correctly build konqueror web browser.
- Updated release number.

* Tue Oct  3 2006 Carl Shaw <carl.shaw@st.com>
- Removed target perl scripts

* Wed Aug  2 2006 Fabio Arnone <fabio.arnone@st.com>
- Updated to 3.3.6 (added -no-cups opt. in ./configure)

* Tue Jul 19 2005 Carl Shaw <carl.shaw@st.com>
- Removed broken link

* Mon Jun  6 2005 Carl Shaw <carl.shaw@st.com>
- Automatically removed dependency opt-out line.  Make sure that moc and qmake are
- target binaries.

* Tue Mar  8 2005 Carl Shaw <carl.shaw@st.com>
- Upgraded to qt embedded version 3.3.4
* Mon Feb  9 2005 Carl Shaw <carl.shaw@st.com>
- Added support for multiple architectures
* Wed Sep  8 2004 Stuart Menefy <stuart.menefy@st.com>
- Automated introduction of %{_stm_pkg_prefix} and %{_pkgname}
* Wed Apr 02 2003 Stuart Menefy <stuart.menefy@st.com>
- Upgraded to 3.1.2, enabled building tools components, and install fonts

* Tue Apr 02 2002 Andrea Cisternino <andrea.cisternino@st.com>
- First version
