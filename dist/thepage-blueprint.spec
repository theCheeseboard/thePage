Name:           thepage-blueprint
Version:        1.0
Release:        1%{?dist}
Summary:        Document Viewer

License:        GPLv3+
URL:            https://github.com/vicr123/thepage
Source0:        https://github.com/vicr123/thepage/archive/%{version}.tar.gz

%if 0%{?fedora} == 32
BuildRequires:  make qt5-devel the-libs-blueprint-devel poppler-qt5-devel
Requires:       qt5 the-libs-blueprint poppler-qt5
%endif

%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel the-libs-blueprint-devel poppler-qt5-devel
Requires:       qt5-qtbase the-libs-blueprint poppler-qt5
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Document viewer using poppler

%prep
%setup

%build
qmake-qt5
make

%install
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

%changelog
* Sun May  3 2020 Victor Tran
- 
