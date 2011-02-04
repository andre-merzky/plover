
all: idl cpp

idl:
	./tex2idl.pl *.tex

cpp: idl
	./idl2cpp.pl *.idl

clean:
	rm -f *.idl
	
