objects = client aws servera serverb serverc
all: $(objects)
.PHONY: all
client: client.cpp
	g++ -o $@ client.cpp -lsocket -lnsl -lresolv
aws: aws.cpp
	g++ -o $@ aws.cpp -lsocket -lnsl -lresolv
servera: aws.cpp
	g++ -o $@ servera.cpp -lsocket -lnsl -lresolv
serverb: serverb.cpp
	g++ -o $@ serverb.cpp -lsocket -lnsl -lresolv
serverc: serverc.cpp
	g++ -o $@ serverc.cpp -lsocket -lnsl -lresolv
.PHONY:clean
clean:
	-rm $(objects)
