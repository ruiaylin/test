all:encoder decoder

#chmod u+x ./mk.sh && source ./mk.sh

encoder:encoder.go
	-go build -gcflags "-N -l" -race -o $@ $<

decoder:decoder.go
	-go build -gcflags "-N -l" -race -o $@ $<

clean:
	rm decoder encoder

