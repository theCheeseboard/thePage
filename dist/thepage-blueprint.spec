Name:           thepage-blueprint
Version:        1.0
Release:        1%{?dist}
Summary:        Document Viewer

License:        GPLv3+
URL:            https://github.com/vicr123/thepage
Source0:        https://github.com/vicr123/thepage/archive/%{version}.tar.gz

%if 0%{?fedora} == 32
BuildRequires:  make qt5-devel the-libs-blueprint-devel poppler-qt5-devel
Requires:       qt5 the-libs-blueprint poppler-qt5 libthepage-blueprint
%endif

%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel the-libs-blueprint-devel poppler-qt5-devel qt5-linguist
Requires:       qt5-qtbase the-libs-blueprint poppler-qt5 libthepage-blueprint
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Document viewer using poppler

%package -n libthepage-blueprint
Summary:        Libraries for %{name} plugins
Conflicts:      libthepage
Provides:       libthepage

%description -n libthepage-blueprint
Libraries for thePage plugins

%prep
%setup

%build
mkdir build
cd build
qmake-qt5
make

%install
cd build
rm -rf $RPM_BUILD_ROOT
#%make_install
make install INSTALL_ROOT=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_bindir}/thepage
%{_datadir}/applications/com.vicr123.thepage.desktop
%{_datadir}/icons/hicolor/scalable/apps/thepage.svg
%{_sysconfdir}/theSuite/thepage/defailts.conf
%{_datadir}/thepage/translations/*
%{_datadir}/thepage/popperplugin/translations/*
%{_libdir}/thepage/plugins/*

%files -n libthepage-blueprint
%{_libdir}/libthepage.so*

%changelog
* Sun May  3 2020 Victor Tran
- 
