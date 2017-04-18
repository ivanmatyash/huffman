Name:           huff
Summary:        Huffman archiver by Ivan Matsiash
Version:        0.1
Release:        1
License:        BSD
Group:          System Environment/Base
URL:            http://github.com/ivanmatyash/huffman.git

Source:         %name-%{version}.tar


%description
%name huffman

%prep
%setup

%build
%__cc heap.c encode.c decode.c main.c -o huff

%install
mkdir -p %{buildroot}%{_bindir}
pwd
ls -l
cp -f huff %{buildroot}%{_bindir}/huff


%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/*

%changelog
* Wed Apr 19 2017 Ivan Matsiash <ivan.matyash97@gmail.com> 0.1-1
- Init rpm
