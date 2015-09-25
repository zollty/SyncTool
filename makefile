# Change the following to suit your environment
CXX=g++
CXXFLAGS+=
LIBS+=
PREFIX=/usr/local

# Do not change anything below this line
SOURCES_WIN=sources/windows/*.cpp
SOURCES_LNX=sources/linux/*.cpp

all: synctool

.phony: install clean windows

synctool: $(SOURCES_LNX)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES_LNX) -o synctool

windows: $(SOURCES_WIN)
	$(CXX) $(CXXFLAGS) $(LIBS) $(SOURCES_WIN) -o synctool.exe

install: synctool
	install -c synctool $(PREFIX)/bin/synctool

clean:
	rm -rf synctool synctool.exe build
