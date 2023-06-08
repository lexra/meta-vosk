CFLAGS=-I../src -O2 -Wno-unused-result -Wno-incompatible-pointer-types
LDFLAGS=-L../src -lvosk -ldl -lpthread -lasound -Wl,-rpath,../src

all: test_vosk test_vosk_speaker test_speech

test_speech: test_speech.o
	$(CC) $^ -o $@ $(LDFLAGS)

test_vosk: test_vosk.o
	$(CC) $^ -o $@ $(LDFLAGS)

test_vosk_speaker: test_vosk_speaker.o
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.a test_vosk test_vosk_speaker test_speech
