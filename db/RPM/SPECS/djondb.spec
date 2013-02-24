Name:	            djondb	
Version:	         0.2.3
Release:	         1%{?dist}
Summary:          djondb	
Group: 				Applications/Databases
License:          GPLv3	
URL:	            http://djondb.com	
Source:           %{name}-%{version}.tar.gz	
BuildRoot:	      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

#BuildRequires: gcc-c++, automake
# BuildRequires:	libxss1, libc6, libx11-6, libantlr3c-3.2-0, libuuid1, libstdc++6, libv8-dev

%description
 djondb is a NoSQL document store created to be 
 easy to use and ready enterprise level. Your 
 data is save with djondb.

%prep
%setup 

%build
autoreconf --install --force
mkdir obj
cd obj
%configure
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%doc

%changelog
